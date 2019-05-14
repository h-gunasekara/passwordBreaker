import itertools
import csv


def password_editor(file):
    contents = file.read().split()
    edited_content = []

    for word in contents:
        if len(word) < 6:
            edited_content.append(fill(word))
        else:
            edited_content.append(word[:6])

    comb_list = []
    for word in edited_content:
        comb_list.extend(case_combinations(word))
        if 'o' in word:
            comb_list.append(letter_replace('o', '0', word))
        if 'i' in word:
            comb_list.append(letter_replace('i', '1', word))
        if 'a' in word:
            comb_list.append(letter_replace('a', '4', word))
    edited_content.extend(comb_list)

    # edited_content = list(dict.fromkeys(edited_content))

    with open("edited_passwords.txt", 'wb') as myfile:
        wr = csv.writer(myfile)
        wr.writerow(edited_content)



def fill(word):
    numbers = "123456"
    spaces = 6 - len(word)
    return word + numbers[:spaces]

def numfill(word):
    return

def case_combinations(word):
    return map(''.join, itertools.product(*((c.upper(), c.lower()) for c in word)))

def letter_replace(old, new, word):
    return word.replace(old, new)





# IDEAS:
    # change everything to caps
    # change the first letter to caps
    # if a word is less than 6 characters long then fill the rest with 1234...
    # can also randomly fill with punctuation
    # sub o with 0
    # sub i with 1
    # sub a with 4


file = open("proj-2_common_passwords.txt", "r")
print(password_editor(file))
