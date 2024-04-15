# Tristan Baskerville
# MAT361
# DigiPen 2023

from NT_functions import modexp
from Euclid import multinv
   
rsaNumToChar = [
  'a', 'b',  'c', 'd', 'e', 'f',  'g', 'h', 'i', 
  'j', 'k',  'l', 'm', 'n', 'o',  'p', 'q', 'r', 
  's', 't',  'u', 'v', 'w', 'x',  'y', 'z', 'A', 
  'B', 'C',  'D', 'E', 'F', 'G',  'H', 'I', 'J', 
  'K', 'L',  'M', 'N', 'O', 'P',  'Q', 'R', 'S',
  'T', 'U',  'V', 'W', 'X', 'Y',  'Z', '0', '1',
  '2', '3',  '4', '5', '6', '7',  '8', '9', '.',
  ',', '!',  '?', ':', ';', '=',  '+', '-', '*',
  '/', '^', '\\', '@', '#', '&',  '(', ')', '[',
  ']', '{',  '}', '$', '%', '_', '\'', ' '
]
rsaNumBase = 11

rsaCharToNum = {
  'a':11, 'b':12,  'c':13, 'd':14, 'e':15, 'f':16,  'g':17, 'h':18, 'i':19, 
  'j':20, 'k':21,  'l':22, 'm':23, 'n':24, 'o':25,  'p':26, 'q':27, 'r':28, 
  's':29, 't':30,  'u':31, 'v':32, 'w':33, 'x':34,  'y':35, 'z':36, 'A':37, 
  'B':38, 'C':39,  'D':40, 'E':41, 'F':42, 'G':43,  'H':44, 'I':45, 'J':46, 
  'K':47, 'L':48,  'M':49, 'N':50, 'O':51, 'P':52,  'Q':53, 'R':54, 'S':55,
  'T':56, 'U':57,  'V':58, 'W':59, 'X':60, 'Y':61,  'Z':62, '0':63, '1':64,
  '2':65, '3':66,  '4':67, '5':68, '6':69, '7':70,  '8':71, '9':72, '.':73,
  ',':74, '!':75,  '?':76, ':':77, ';':78, '=':79,  '+':80, '-':81, '*':82,
  '/':83, '^':84, '\\':85, '@':86, '#':87, '&':88,  '(':89, ')':90, '[':91,
  ']':92, '{':93,  '}':94, '$':95, '%':96, '_':97, '\'':98, ' ':99
}

def rsaNumToString(n):
    text = ''
    string = str(n)

    for i in range(0, len(string), 2):
      a = int(string[i:i+2]) - rsaNumBase
      text += rsaNumToChar[a]
    return text

def rsaExp(p, q, e): # returns d
    # phi(N) = phi(p) * phi(q)
    # phi(prime) = prime - 1
    return multinv(e, (p - 1) * (q - 1))

def rsaEncode(message, n, e, blocksize):
  blocks = []
  output = ''
  read = 0
  for c in message:
    output += str(rsaCharToNum[c])
    read += 1

    if read >= blocksize:
      blocks.append(int(output))
      output = '' 
      read = 0
  
  if output != '': # add remaining value if it exists
    blocks.append(int(output))

  # encryption pass
  for index in range(0, len(blocks)):
    print(blocks[index])
    blocks[index] = modexp(blocks[index], e, n)
  return blocks

def rsaDecode(blocks, p, q, e):
  output = []
  d = rsaExp(p, q, e)
  for number in blocks:
    result = modexp(number, d, p*q)
    output.append(rsaNumToString(result))
  return output

def rsaDecoded(blocks, p, q, e):
    result = rsaDecode(blocks, p, q, e)
    output = ''
    for text in result:
        output += text
    return output
