# Tristan Baskerville
# MAT361
# DigiPen 2023

# only used for the first sqrt(n) primes
def isPrime(n):
    if n < 2:
        return False
    if n % 2 == 0:
        return n == 2
    k = 3
    while k * k <= n:  # only odd numbers
        if n % k == 0:
            return False
        k += 2
    return True

def prime_base_neg(primeBound):
    primes = [-1] # build base of primes
    for p in range(2, primeBound):
        if isPrime(p):
            primes.append(p)
    return primes

def prime_base(primeBound):
    primes = [] # build base of primes
    for p in range(2, primeBound):
        if isPrime(p):
            primes.append(p)
    return primes