"""
Title: Password Creator
Author: Hamish Gunasekara

creates file (edited_passwords.txt) that are all the varied passwords that I have made up
crack.c then processes these passwords to compare hashes.
"""


import itertools
import csv

output = []


def password_editor(file):
    """
    As if its the main function for the passwordcreator.py. The most important thing it does is output a .txt (edited_passwords.txt) file of
    varied passwords.
    Input: "file" - common_passwords.txt from Jun haog
    Returns: "len(edited_content)" - the number of different passwords I have come up with (3,113,956)
    """

    # Splits file words up
    contents = file.read().split()
    edited_content = []

    # iterates through all words of common_passwords.txt
    for word in contents:
        # If the password is less than 6 characters
        if len(word) < 6:
            # Fill the remaining spaces with the cartesian product of all
            # numbers that fit
            edited_content.extend(fill(word))
            # Remove all numbers that are in the common password
            digitstripped = ''.join(i for i in word if not i.isdigit())
            # If there are numbers removed then process more
            if digitstripped != '' and digitstripped != word:
                # Fill the remaining spaces with the cartesian product of all
                # numbers that fit
                edited_content.extend(fill(digitstripped))
        else:
            # If the password is longer than 6 characters then just take the
            # first 6 chars
            edited_content.append(word[:6])
            # Remove all numbers that are in the common password
            digitstripped = ''.join(i for i in word[:6] if not i.isdigit())
            # If there are numbers removed then process more
            if digitstripped != '' and digitstripped != word:
                # Fill the remaining spaces with the cartesian product of all
                # numbers that fit
                edited_content.extend(fill(digitstripped))

    comb_list1 = []
    #  puts a capital letter in front of each word
    for word in edited_content:
        comb_list1.append(word.capitalize())
    edited_content.extend(comb_list1)

    comb_list2 = []
    # Subs all letter number subsitutions (e.g 's' and 5)
    for word in edited_content:
        sub(0, word)
    edited_content.extend(output)
    comb_list = []

    # remove duplicates
    edited_content = list(dict.fromkeys(edited_content))
    # Put my own name in
    edited_content.append("hgunas")

    # write to file
    with open("edited_passwords.txt", 'wb') as myfile:
        wr = csv.writer(myfile, delimiter=' ')
        wr.writerow(edited_content)

    return len(edited_content)


def sub(i, word):
    """
    Recursively find all combinations of letter subsitutions
    Input:
        i - current index of word
        word - the word we are subsituting

    Returns:
        word - the final word
    """
    if i >= 6:
        return word
    else:
        # These are all the different substitutions
        # I will comment one and the rest are the same
        if 'o' in word[i]:
            # replace letter
            new_word = letter_replace('0', i, word)
            # Output it to a list
            # This list is global
            # (I know it is bad practice by it was the best way I found)
            output.append(new_word)
            # now check the new word to see if more subs can be made
            sub(i + 1, new_word)

        if 'i' in word[i]:
            new_word = letter_replace('1', i, word)
            output.append(new_word)
            sub(i + 1, new_word)

        if 'l' in word[i]:
            new_word = letter_replace('1', i, word)
            output.append(new_word)
            sub(i + 1, new_word)

        if 'l' in word[i]:
            new_word = letter_replace('7', i, word)
            output.append(new_word)
            sub(i + 1, new_word)

        if 'a' in word[i]:
            new_word = letter_replace('4', i, word)
            output.append(new_word)
            sub(i + 1, new_word)

        if 's' in word[i]:
            new_word = letter_replace('5', i, word)
            output.append(new_word)
            sub(i + 1, new_word)

        if 'e' in word[i]:
            new_word = letter_replace('3', i, word)
            output.append(new_word)
            sub(i + 1, new_word)



def fill(word):
    """
    Fills word with cartesian product of all numbers
    Input:
        word - the word being filled

    Returns:
        final - the list fo all filled words
    """
    numbers = "1234567890"
    spaces = 6 - len(word)
    # finds the cartesian product
    permutes = [''.join(i) for i in itertools.product(numbers, repeat=spaces)]
    # adds it to the back of word
    final = [word + permute for permute in permutes]
    return final


def letter_replace(new, index, word):
    """
    Replaces a character in the word
    Input:
        new - new character that is going in
        index - where that character is going in
        word - the word that is being changed
    """
    # Makes the string a list
    final = list(word)
    # changes the letter at the given index
    final[index] = new
    # joins it back as a string
    return ''.join(final)


file = open("proj-2_common_passwords.txt", "r")
print(password_editor(file))
