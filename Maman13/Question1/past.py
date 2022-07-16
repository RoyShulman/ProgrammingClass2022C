words = ["adopt", "bake", "beam", "cook", "time", "grill", "waved", "hire"]
past_tense = []
for word in words:
    if word.endswith('e'):
        past_tense_word = word + 'd'
    elif word.endswith('ed'):
        past_tense_word = word
    else:
        past_tense_word = word + 'ed'
    past_tense.append(past_tense_word)
