# Load required libraries
library(data.table)
library(caret)
library(randomForest)
library(ggplot2)

# Step 1: Read all .tsd files and assign class labels based on gesture names
read_all_data <- function(folder_path) {
  file_list <- list.files(folder_path, pattern = "\\.tsd$", full.names = TRUE, recursive = TRUE)
  
  # Extract unique gesture names to create class mapping
  gesture_names <- unique(sapply(file_list, function(file) {
    basename_file <- basename(file)
    # Remove the -1, -2, -3 suffix and .tsd extension
    gesture_name <- gsub("-[1-3]\\.tsd$", "", basename_file)
    return(gesture_name)
  }))
  
  # Sort gesture names for consistent class assignment
  gesture_names <- sort(gesture_names)
  print(paste("Found", length(gesture_names), "unique gestures"))
  print("Gesture names:")
  print(gesture_names)
  
  # Create class mapping (0-94 for 95 gestures)
  class_mapping <- setNames(0:(length(gesture_names) - 1), gesture_names)
  
  all_data <- rbindlist(lapply(file_list, function(file) {
    tryCatch({
      data <- fread(file, header = FALSE, sep = "\t")  # No header for .tsd files
      
      # Extract gesture name from filename
      basename_file <- basename(file)
      gesture_name <- gsub("-[1-3]\\.tsd$", "", basename_file)
      
      # Assign class label
      class_label <- class_mapping[gesture_name]
      
      # Add class label as the last column
      data$class <- class_label
      
      print(paste("Loaded:", file, "- Gesture:", gesture_name, "- Class:", class_label))
      return(data)
    }, error = function(e) {
      print(paste("Error reading file:", file, ":", e))
      return(NULL)
    })
  }))
  
  print("All data successfully loaded.")
  print(paste("Total samples:", nrow(all_data)))
  print(paste("Classes range from 0 to", max(all_data$class)))
  
  return(list(data = all_data, class_mapping = class_mapping))
}

# Step 2: Preprocess the data
preprocess_data <- function(data) {
  # Assuming the last column is the class label
  X <- data[, -ncol(data), with = FALSE]  # Features (all columns except the last)
  y <- data[[ncol(data)]]                 # Labels (last column)
  return(list(X = X, y = y))
}

# Step 3: Apply PCA for dimensionality reduction
apply_pca <- function(X, n_components = 10) {
  # Perform PCA
  pca_result <- prcomp(X, center = TRUE, scale. = TRUE)
  
  # Adjust n_components if it exceeds the number of features
  max_components <- ncol(pca_result$x)
  if (n_components > max_components) {
    n_components <- max_components
    print(paste("Adjusted n_components to", n_components, "due to limited features."))
  }
  
  # Select the top `n_components` principal components
  X_pca <- as.data.frame(pca_result$x[, 1:n_components])
  print(paste("PCA applied. Reduced to", n_components, "components."))
  
  return(X_pca)
}

# Each file is a sign (95 total), so for every three files in a folder try
# to index 0-94

# Step 4: Train separate classifiers for position, rotation, and gestures
train_position_classifier <- function(X, y) {
  # Assuming first 3 columns are position data (x, y, z)
  X_position <- X[, 1:3, with = FALSE]
  
  # Split data
  set.seed(42)
  train_index <- createDataPartition(y, p = 0.8, list = FALSE)
  X_train <- X_position[train_index, ]
  y_train <- y[train_index]
  X_test <- X_position[-train_index, ]
  y_test <- y[-train_index]
  
  # Train classifier
  classifier <- randomForest(x = X_train, y = as.factor(y_train), ntree = 100)
  
  # Evaluate
  y_pred <- predict(classifier, X_test)
  accuracy <- mean(y_pred == y_test)
  print(paste("Position Classifier Accuracy:", round(accuracy, 4)))
  
  return(list(model = classifier, test_data = X_test, test_labels = y_test))
}

train_rotation_classifier <- function(X, y) {
  # Assuming columns 4-6 are rotation data (roll, pitch, yaw)
  X_rotation <- X[, 4:6, with = FALSE]
  
  # Split data
  set.seed(42)
  train_index <- createDataPartition(y, p = 0.8, list = FALSE)
  X_train <- X_rotation[train_index, ]
  y_train <- y[train_index]
  X_test <- X_rotation[-train_index, ]
  y_test <- y[-train_index]
  
  # Train classifier
  classifier <- randomForest(x = X_train, y = as.factor(y_train), ntree = 100)
  
  # Evaluate
  y_pred <- predict(classifier, X_test)
  accuracy <- mean(y_pred == y_test)
  print(paste("Rotation Classifier Accuracy:", round(accuracy, 4)))
  
  return(list(model = classifier, test_data = X_test, test_labels = y_test))
}

train_gesture_classifier <- function(X, y) {
  # Assuming columns 7+ are gesture/sensor data
  X_gesture <- X[, 7:ncol(X), with = FALSE]
  
  # Split data
  set.seed(42)
  train_index <- createDataPartition(y, p = 0.8, list = FALSE)
  X_train <- X_gesture[train_index, ]
  y_train <- y[train_index]
  X_test <- X_gesture[-train_index, ]
  y_test <- y[-train_index]
  
  # Train classifier
  classifier <- randomForest(x = X_train, y = as.factor(y_train), ntree = 100)
  
  # Evaluate
  y_pred <- predict(classifier, X_test)
  accuracy <- mean(y_pred == y_test)
  print(paste("Gesture Classifier Accuracy:", round(accuracy, 4)))
  
  return(list(model = classifier, test_data = X_test, test_labels = y_test))
}

# Step 5: Combine predictions from all classifiers
combine_predictions <- function(pos_model, rot_model, gest_model, X, y) {
  # Split data the same way for consistency
  set.seed(42)
  train_index <- createDataPartition(y, p = 0.8, list = FALSE)
  X_test <- X[-train_index, ]
  y_test <- y[-train_index]
  
  # Get predictions from each model
  pos_pred <- predict(pos_model$model, X_test[, 1:3, with = FALSE], type = "prob")
  rot_pred <- predict(rot_model$model, X_test[, 4:6, with = FALSE], type = "prob")
  gest_pred <- predict(gest_model$model, X_test[, 7:ncol(X_test), with = FALSE], type = "prob")
  
  # Weighted average combination (you can adjust weights)
  pos_weight <- 0.2
  rot_weight <- 0.3
  gest_weight <- 0.5
  
  # Combine probability predictions
  combined_pred <- pos_weight * pos_pred + rot_weight * rot_pred + gest_weight * gest_pred
  
  # Get final predictions
  final_pred <- as.factor(colnames(combined_pred)[apply(combined_pred, 1, which.max)])
  
  # Evaluate combined model
  accuracy <- mean(final_pred == y_test)
  print(paste("Combined Classifier Accuracy:", round(accuracy, 4)))
  
  # Create confusion matrix
  confusion <- confusionMatrix(final_pred, as.factor(y_test))
  print("Combined Model Classification Report:")
  print(confusion)
  
  # Plot confusion matrix (suppress binding warnings)
  confusion_df <- as.data.frame(confusion$table)
  names(confusion_df) <- c("Reference", "Prediction", "Freq")
  
  # Create the plot (R CMD check warnings can be ignored for interactive use)
  suppressWarnings({
    heatmap_plot <- ggplot(confusion_df) +
      geom_tile(aes(x = Reference, y = Prediction, fill = Freq)) +
      geom_text(aes(x = Reference, y = Prediction, label = Freq), color = "white", size = 2) +
      scale_fill_gradient(low = "blue", high = "red") +
      labs(title = "Combined Model Confusion Matrix", x = "Actual Class", y = "Predicted Class") +
      theme_minimal() +
      theme(axis.text.x = element_text(angle = 45, hjust = 1, size = 8),
            axis.text.y = element_text(size = 8))
  })
  
  print(heatmap_plot)
  
  return(list(
    combined_accuracy = accuracy,
    confusion_matrix = confusion,
    individual_models = list(position = pos_model, rotation = rot_model, gesture = gest_model)
  ))
}

# Main function
main <- function() {
  folder_path <- "."  # Current directory containing tctodd folders
  result <- read_all_data(folder_path)
  
  if (!is.null(result)) {
    processed <- preprocess_data(result$data)
    X <- processed$X
    y <- processed$y
    
    print(paste("Original data shape:", paste(dim(X), collapse = " x ")))
    print(paste("Number of features:", ncol(X)))
    print(paste("Number of samples:", nrow(X)))
    print(paste("Number of classes:", length(unique(y))))
    
    # Train individual classifiers
    print("\n=== Training Position Classifier ===")
    pos_model <- train_position_classifier(X, y)
    
    print("\n=== Training Rotation Classifier ===")
    rot_model <- train_rotation_classifier(X, y)
    
    print("\n=== Training Gesture Classifier ===")
    gest_model <- train_gesture_classifier(X, y)
    
    # Combine all classifiers
    print("\n=== Combining All Classifiers ===")
    combined_results <- combine_predictions(pos_model, rot_model, gest_model, X, y)
    
    print("\n=== Summary ===")
    print("Classification complete!")
    print(paste("Position model accuracy:", round(mean(predict(pos_model$model, pos_model$test_data) == pos_model$test_labels), 4)))
    print(paste("Rotation model accuracy:", round(mean(predict(rot_model$model, rot_model$test_data) == rot_model$test_labels), 4)))
    print(paste("Gesture model accuracy:", round(mean(predict(gest_model$model, gest_model$test_data) == gest_model$test_labels), 4)))
    print(paste("Combined model accuracy:", round(combined_results$combined_accuracy, 4)))
    
    return(combined_results)
  }
}

# Run the main function
main()