# Tristan Baskerville
# MAT361
# DigiPen 2023

import random
from NT_functions import modexp


def strong_fermat_test(number, base):
    # return modexp(base, number - 1, number)
    k = 0
    s = number - 1
    while s % 2 == 0:
        s //= 2
        k += 1

    b0 = modexp(base, s, number)
    if b0 == 1:
        return 1

    bim1 = lambda i: b0 if i == 0 else modexp(bim1(i - 1), 2, number)
    for i in range(1, k):
        bi = bim1(i)
        if bi == (-1 % number):
            return 1
    return 0


def rabin_miller(number, m=200):
    bases = range(2, number)
    randomBases = random.sample(bases, m)
    for base in randomBases:
        if strong_fermat_test(number, base) == 0:
            return 0
    return 1


from TrialDivision import trial_division
from PrimeHelpers import isPrime


def lehmer(N, b=2):
    if modexp(b, N - 1, N) != 1:
        return False
    divisors, exponents = trial_division(N - 1, limit=1000000)
    print(f'Factoring N - 1 using trial division:\nDivisors: {divisors}\nExponents: {exponents}')
    for q in divisors:
        assert isPrime(q), "Non-prime returned from trial division."
    results = [("b", b)]
    results.append(("b^(n-1)", modexp(b, N - 1, N)))
    is_prime = True
    for q in divisors:
        result = modexp(b, (N - 1) // q, N)
        results.append((f"b^(n-1)/{q}", result))
        if result == 1:
            is_prime = False
            break
    print(f'Using Lehmers test with b = {b}, we get:\n{results}')
    print(f'So that the number N = {N} is {'prime!' if is_prime else 'not prime!'}')
    return is_prime
