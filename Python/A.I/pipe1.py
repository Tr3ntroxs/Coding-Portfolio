from transformers import pipeline
from transformers import AutoTokenizer, AutoModelForSequenceClassification

#https://www.youtube.com/watch?v=GSt00_-0ncQ&t=193s

classifier = pipeline("sentiment-analysis")
res = classifier("Welcome!")

print(res)