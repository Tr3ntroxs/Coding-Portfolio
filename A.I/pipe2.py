from transformers import pipeline

generator = pipeline("text-generation", model="distilgpt2")

res = generator("Oh, oh, listen to the music", max_length=30, num_return_seqeuences=2)


print(res)