# Tristan Baskerville
# MAT361
# DigiPen 2023

from NT_functions import modexp
from Euclid import multinv
from RSA import rsaCharToNum, rsaNumToString

def elgamal_encrypt(message, p, r, ra, b, blocksize):
    rb = modexp(r, b, p)
    rab = modexp(ra, b, p)
    output = ''
    blocks = [rb]
    read = 0
    for c in message:
        output += str(rsaCharToNum[c])
        read += 1

        if read >= blocksize:
          #print(int(output))
          blocks.append((int(output) * rab) % p)
          output = '' 
          read = 0

    if output != '': # add remaining value if it exists
        #print(int(output))
        blocks.append((int(output) * rab) % p)
        
    return blocks

def elgamal_decrypt(blocks, p, a):
    rb = blocks[0]
    rab = modexp(rb, a, p)
    rab_inv = multinv(rab, p)

    output = []
    for i in range(1, len(blocks)):
        result = (blocks[i] * rab * rab_inv) % p
        result = (result * rab_inv) % p
        output.append(rsaNumToString(result))
    return output