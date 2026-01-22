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

# Step 4: Train one classifier across position, rotation, and gestures
train_gesture_classifier <- function(X, y) {
  # Use all columns for gesture features
  X_gesture <- X

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

# Step 5: Evaluate the gesture classifier and create visualizations
evaluate_classifier <- function(gest_model, X, y) {
  # Split data the same way for consistency
  set.seed(42)
  train_index <- createDataPartition(y, p = 0.8, list = FALSE)
  X_test <- X[-train_index, ]
  y_test <- y[-train_index]
  
  # Get predictions
  y_pred <- predict(gest_model$model, X_test)
  
  # Evaluate model
  accuracy <- mean(y_pred == y_test)
  print(paste("Gesture Classifier Accuracy:", round(accuracy, 4)))
  
  # Create confusion matrix
  confusion <- confusionMatrix(y_pred, as.factor(y_test))
  print("Gesture Classifier Classification Report:")
  print(confusion)
  
  # Plot confusion matrix
  confusion_df <- as.data.frame(confusion$table)
  names(confusion_df) <- c("Reference", "Prediction", "Freq")
  
  # Create the plot
  suppressWarnings({
    heatmap_plot <- ggplot(confusion_df) +
      geom_tile(aes(x = Reference, y = Prediction, fill = Freq)) +
      geom_text(aes(x = Reference, y = Prediction, label = Freq), color = "white", size = 2) +
      scale_fill_gradient(low = "blue", high = "red") +
      labs(title = "Gesture Classifier Confusion Matrix", x = "Actual Class", y = "Predicted Class") +
      theme_minimal() +
      theme(axis.text.x = element_text(angle = 45, hjust = 1, size = 8),
            axis.text.y = element_text(size = 8))
  })
  
  print(heatmap_plot)
  
  return(list(
    accuracy = accuracy,
    confusion_matrix = confusion,
    model = gest_model
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
    
    print("\n=== Training Gesture Classifier ===")
    gest_model <- train_gesture_classifier(X, y)
    
    # Evaluate the classifier
    print("\n=== Evaluating Gesture Classifier ===")
    results <- evaluate_classifier(gest_model, X, y)
    
    print("\n=== Summary ===")
    print("Classification complete!")
    print(paste("Gesture model accuracy:", round(results$accuracy, 4)))
    
    return(results)
  }
}

# Run the main function
main()