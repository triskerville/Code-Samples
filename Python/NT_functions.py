# Tristan Baskerville
# MAT361
# DigiPen 2023

# Number Theory Functions
import math
from PyHelpers import timed_function

def modexp(a,x,m):
    a0 = a
    xb2 = baseb(x,2)
    end = len(xb2)
    result = 1
    # manually do the first element since it's not getting squared
    if xb2[end - 1] == 1:
        result = (result * a)
    #print (a, result)
    for i in range(end - 2, -1, -1):
        a = a**2 % m
        if (xb2[i] == 1):
            result = (result * a)
        #print (a, result)
            
    return result % m

from Euclid import multinv

def baseb(n,b):
    # n = q * b + r
    coeffs = []
    while n > 0:
      q = n // b
      r = n % b
      # print("{} = {} * {} + {}".format(n,q,b,r))
      coeffs.insert(0, r)
      n = q
    
    return coeffs


@timed_function # sometimes the printing is annoying lol
def baby_giant_timed(number, root, prime):
    baby_giant(number, root, prime)
    
def baby_giant(number, root, prime):
    N = int(math.ceil(math.sqrt(prime - 1)))
    rN = modexp(root, N, prime)
    R = multinv(rN, prime)
    
    rList = [ 1 ]
    RList = [ number ]
    
    for n in range(0, N-1):
        rList.append((rList[n] * root) % prime)
        RList.append((RList[n] * R) % prime)

    for i in range(0, len(rList)):
        for j in range(0, len(RList)):
            if rList[i] == RList[j]:
                print(f"i: {i}, j: {j}, N: {N}, i + j * N = {i+j*N}")
                return i + j * N
            
def powertable(modulus):
    # a  0 1 2 3 ... modulus-1
    max_width = len(str(modulus)) + 1
    for p in range(1, modulus):
        print(f'a^{p} '.center(2+max_width), end='')
        for a in range(modulus):
            print(f'{modexp(a, p, modulus)}'.center(max_width), end='')
        print()
    
