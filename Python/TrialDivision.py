# Tristan Baskerville
# MAT361
# DigiPen 2023

from Sieve import sieve
from Euclid import euclid
from NT_functions import modexp
from PyHelpers import timed_function
from PrimeHelpers import isPrime

def trial_division(n, limit=1000000):
  # sieve up to 1m is slow, early out if possible
  primes = sieve(1, min(n, limit))
  divisors = []
  i = 0
  divisions = 0
  
  while i < len(primes):
    p = primes[i]
    if (n == 1): break
    if (n % p == 0):
      divisors.append(p)
      n = n // p # don't let python use floating point division
    else:
      i += 1
    divisions += 1
    
  if (n != 1):
      #divisors.append("'{}'".format(n))
      divisors.append(n)
      
  last = -1
  factors = []
  exponents = []
  for factor in divisors:
    if (factor != last):
        factors.append(factor)
        exponents.append(1)
        last = factor
    else:
        exponents[len(exponents)-1] += 1

  
  #print('Attempted {} divisions.'.format(divisions))
  #print(*factors, sep=" * ")
  return factors, exponents

def prime_divisors(n):
  output = []
  divisor = 2
  while divisor < n:
    if isPrime(divisor) and n % divisor == 0:
      output.append(divisor)
    divisor += 1
  return output

def primitive_root(n):

  def primitive_root_aux(n, r):
    phi_n = phi(n)
    for q in prime_divisors(phi_n):
      result = modexp(r, phi_n // q, n)
      if result == 1: break
    else: return r
    return primitive_root_aux(n, r+1)

  return primitive_root_aux(n, 2)

@timed_function
def order_timed(a,n):
   return order(a, n)

def order(a,n):
    p = phi(n)
    gcd, x0, y0 = euclid(a, n)
    if (gcd != 1): return "gcd({}, {}) != 1".format(a,n)
    
    for i in range(1,p+1):
        if (p % i != 0): continue
        if modexp(a, i, n) == 1: return i
    
def phi(n):
    if isPrime(n): 
        return n - 1
    factors, exponents = trial_division(n)
    for factor in factors:
        n *= 1 - (1 / factor)
    return int(n)

@timed_function
def index_timed(number, root, modulus):
    index(number, root, modulus)
    
def index(number, root, modulus):
    product = 1
    for exponent in range(1, modulus):
        product = (root * product) % modulus
        if product == number:
            return exponent