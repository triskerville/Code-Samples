# Tristan Baskerville
# MAT361
# DigiPen 2023

import math
from PrimeHelpers import isPrime, prime_base, prime_base_neg
from PyHelpers import format_table, factor_from_primes, superscript
from Euclid import euclid_gcd
from NT_functions import modexp


def fermat_factorization(number):
    if isPrime(number):
        print(f"Number {number} is already prime.")
        return
    if number % 2 == 0:
        m = 0
        while number % 2 == 0:
            number //= 2
            m += 1
        print(f"2^{m} * {number}")

    fermat_factorization_aux(number)


def fermat_factorization_aux(number):
    def is_square(N): # fast way to check if a number is a square
        lastdigits = [0, 1, 4, 5, 6, 9]
        last2digits = [0, 1, 4, 6, 9, 16, 21, 24, 25, 29, 36, 41, 44, 49, 56, 61, 64, 69, 76, 81, 84, 89, 96]
        return (
            (N % 10 in lastdigits)
            and (N % 100 in last2digits)
            and (math.isqrt(N)**2 == N)
        )

    limit = 1500000000  # limit loop to 1,500,000,000 steps
    root = math.ceil(math.sqrt(number))
    for i in range(0, limit):
        difference = (root + i) ** 2 - number
        if is_square(difference):
            a = root + i
            b = math.isqrt(difference)
            print(f"steps: {i + 1}, a = {a}, b = {b}, a-b = {a-b}, a+b = {a+b}")
            return

    print(f"Exceeded {limit} steps")


def pollard_rho(N, a=1, M=1000):
    f = lambda x: x * x + a
    x0 = 2
    y0 = f(f(x0))

    for i in range(1, M):
        x0 = f(x0) % N
        y0 = f(f(y0)) % N
        gcd = euclid_gcd(x0 - y0, N)
        # print(f'x0: {x0}, y0: {y0}, gcd(x0 - y0, N): {gcd}')

        if gcd != 1 and gcd != N:
            return gcd, N // gcd


def pollard_Pminus1(N, a, M=1000):
    for i in range(2, M):
        a = modexp(a, i, N)
        gcd = euclid_gcd(a - 1, N)
        if gcd != 1 and gcd != N:
            return gcd, N // gcd


def continued_fraction(N, primeBound=25, iterations=50):
    an = math.isqrt(N)
    An, Bn = 0, 1
    pPrev, pCurr = 0, 1
    qPrev, qCurr = 1, 0

    primes = prime_base(primeBound)

    table = []
    table.append(["i", "Ai", "Bi", "ai", "pi", "qi"])
    table.append([0, An, Bn, an, pCurr, qCurr])
    BnTable = []
    BnTable.append(["Bn", "Factored"])
    BnTable.append([1, 1])
    for i in range(1, iterations):
        Ai = an * Bn - An
        Bi = (N - Ai**2) // Bn
        ai = math.floor((Ai + math.sqrt(N)) / Bi)
        pi = ai * pCurr + pPrev
        qi = ai * qCurr + qPrev
        table.append([i, Ai, Bi, ai, pi, qi])
        exponents, remainder = factor_from_primes(Bi, primes)
        factored = ""
        for idx, e in enumerate(exponents):
            if e != 0:
                if factored != "":
                    factored += " * "
                factored += f"{primes[idx]}{superscript(e)}"
        if remainder != 1:
            if factored != "":
                factored += " * "
            factored += f"{remainder}"

        BnTable.append([Bi, factored])
        # exchange values for next iteration
        An, Bn, an = Ai, Bi, ai
        pPrev, pCurr = pCurr, pi
        qPrev, qCurr = qCurr, qi

    format_table(table)
    print()
    # format_table(BnTable)


def mod2(matrix):
    numRows = len(matrix)
    numCols = len(matrix[0])
    for iRow in range(numRows):
        for iCol in range(numCols):
            matrix[iRow][iCol] %= 2
    return matrix


from IndexCalculus import modref, modrref
