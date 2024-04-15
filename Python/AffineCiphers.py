# Tristan Baskerville
# MAT361
# DigiPen 2023

from Euclid import euclid, multinv

def affine_cipher(a, b, text):
    nums = []
    for c in text:
        # we want 'a' to map to 0, not its ascii value
        nums.append(ord(c) - ord("a"))

    result = ""
    for i in range(0, len(nums)):
        nums[i] = (a * nums[i] + b) % 26
        result += chr(nums[i] + ord("a"))

    return result


def affine_decipher(a, b, text):
    gcd, x0, y0 = euclid(a, 26, False)
    if gcd != 1:
        return print("gcd({},26) != 1".format(a))

    a1 = multinv(a, 26)
    b1 = a1 * (-b)

    return affine_cipher(a1, b1, text)

ceasarCharToNum = {
    'a':0, 'b':1, 'c':2, 'd':3, 'e':4, 'f':5,
    'g':6, 'h':7, 'i':8, 'j':9, 'k':10, 'l':11,
    'm':12, 'n':13, 'o':14, 'p':15, 'q':16, 'r':17,
    's':18, 't':19, 'u':20, 'v':21, 'w':22, 'x':23,
    'y':24, 'z':25
}

ceasarNumToChar = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']

def ceasar_cipher(b, message):
    outputMessage = ''
    for char in message:
        shift = (ceasarCharToNum[char] + b) % 26
        outputMessage += ceasarNumToChar[shift]
    return outputMessage

def ceasar_decipher(b, message):
    outputMessage = ''
    for char in message:
        shift = (ceasarCharToNum[char] - b) % 26
        outputMessage += ceasarNumToChar[shift]
    return outputMessage