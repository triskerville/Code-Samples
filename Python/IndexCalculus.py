# Tristan Baskerville
# MAT361
# DigiPen 2023

from Euclid import euclid, euclid_gcd, multinv
from NT_functions import modexp
from PrimeHelpers import isPrime, prime_base
from PyHelpers import factor_from_primes, format_table, timed_function

def createMat(m, n):
    output = []
    for row in range(m):
        output.append([0] * n)
    return output

def identityMat(m, n):
    output = createMat(m, n)
    row,col = 0,0
    while row < m and col < n:
        output[row][col] = 1
        row += 1
        col += 1
    return output

# Since column vectors are a pain to write, we treat them as lists separately
def multVec(A, v):
    def dot(a, b):
        assert(len(a) == len(b))
        total = 0
        for i in range(len(a)):
            total += a[i] * b[i]
        return total
            
    output = []
    for i in range(len(A)):
        output.append(dot(A[i], v))
    return output
    
# Matrices as a list of rows (list of values)
def multMat(A, B):
    Am = len(A)
    An = len(A[0])
    Bm = len(B)
    Bn = len(B[0])
    assert(An == Bm)
    
    output = createMat(Am, Bn)
    for row in range(Am):
        for col in range(Bn):            
            total = 0
            for i in range(An):
                total += A[row][i] * B[i][col]
            output[row][col] = total
    return output        

def find_pivot(matrix, iRow, iColumn, modulus):
    def find_one_pivot(matrix, iRow, iColumn):
        numRows = len(matrix)
        for i in range(numRows-1, iRow-1, -1): 
            if matrix[i][iColumn] == 1: return i
        return -1

    def find_gcd_one_pivot(matrix, iRow, iColumn, modulus):
        numRows = len(matrix)
        for i in range(numRows-1, iRow-1, -1): 
            if euclid_gcd(matrix[i][iColumn], modulus) == 1: return i
        return -1

    def find_smallest_pivot(matrix, iRow, iColumn, modulus):
        numRows = len(matrix)
        smallest = modulus # since everything is % modulus, nothing can be greater than modulus lol
        index = -1
        for i in range(numRows-1, iRow-1, -1): 
            if matrix[i][iColumn] < smallest and matrix[i][iColumn] != 0:
                smallest = modulus
                index = i
        return index
    
    def find_nonzero_pivot(matrix, iRow, iColumn):
        numRows = len(matrix)
        for i in range(numRows-1, iRow-1, -1): 
            if matrix[i][iColumn] != 0: return i
        return -1
    
    pivot = find_one_pivot(matrix, iRow, iColumn)
    if pivot != -1: return pivot
    pivot = find_gcd_one_pivot(matrix, iRow, iColumn, modulus)
    if pivot != -1: return pivot
    pivot = find_smallest_pivot(matrix, iRow, iColumn, modulus)
    if pivot != -1: return pivot
    pivot = find_nonzero_pivot(matrix, iRow, iColumn)
    return pivot

# reduce A to U
def reduce_rows(matrix, iRow, iColumn, modulus):
    numRows    = len(matrix)
    numColumns = len(matrix[0])

    pivot = find_pivot(matrix, iRow, iColumn, modulus)
    if pivot == -1: return matrix # no pivot found, likely a column of zero's
    if pivot != iRow: # iRow could already have the best pivot
        matrix[pivot], matrix[iRow] = matrix[iRow], matrix[pivot]
    
    if matrix[iRow][iColumn] != 1: # reduce pivot to 1 (if possible)
        if euclid_gcd(matrix[iRow][iColumn], modulus) == 1:
            k = multinv(matrix[iRow][iColumn], modulus)
            for i in range(iColumn, numColumns):
                matrix[iRow][i] = (matrix[iRow][i] * k) % modulus
    # print("xxxxxxxxxxxxxxxxxxx")
    # format_table(matrix)
    # print("xxxxxxxxxxxxxxxxxxx")

    pivotRow = iRow
    iRow += 1
    # reduce rows below pivot to be 0
    while iRow < numRows:
        # skip rows that are already 0
        if matrix[iRow][iColumn] == 0:
            iRow += 1
            continue
        
        if matrix[pivotRow][iColumn] != 1:
            gcd, x0, y0 = euclid(matrix[pivotRow][iColumn], matrix[iRow][iColumn])
            for i in range(iColumn, numColumns):
                matrix[pivotRow][i] = (matrix[pivotRow][i] * x0) % modulus
                matrix[pivotRow][i] = (y0 * matrix[iRow][i] + matrix[pivotRow][i]) % modulus

        k = matrix[iRow][iColumn] // matrix[pivotRow][iColumn]
        for i in range(iColumn, numColumns):
            matrix[iRow][i] = (-k * matrix[pivotRow][i] + matrix[iRow][i]) % modulus

        iRow += 1

    return matrix

def upper_triangle(matrix, iRow, iColumn, modulus):
    numRows    = len(matrix)
    numColumns = len(matrix[0])

    if matrix[iRow][iColumn] == 0: return matrix

    pivotRow = iRow
    iRow -= 1
    while iRow >= 0:
        if matrix[iRow][iColumn] == 0:
            iRow -= 1
            continue

        if matrix[pivotRow][iColumn] != 1:
            gcd, x0, y0 = euclid(matrix[pivotRow][iColumn], matrix[iRow][iColumn])
            for i in range(iColumn, numColumns):
                matrix[pivotRow][i] = (matrix[pivotRow][i] * x0) % modulus
                matrix[pivotRow][i] = (y0 * matrix[iRow][i] + matrix[pivotRow][i]) % modulus

        k = matrix[iRow][iColumn] // matrix[pivotRow][iColumn]
        for i in range(iColumn, numColumns):
            matrix[iRow][i] = (-k * matrix[pivotRow][i] + matrix[iRow][i]) % modulus
        # print("xxxxxxxxxxxxxxxxxxx")
        # format_table(matrix)
        # print("xxxxxxxxxxxxxxxxxxx")
        iRow -= 1

    return matrix



def delete_zero_rows(matrix):
    rows = len(matrix)
    cols = len(matrix[0])
    for i in range(rows-1, -1, -1):
        for j in range(cols):
            if matrix[i][j] != 0: break
        else:
            del matrix[i]
    return matrix    

def modref(matrix, modulus):
    rows = len(matrix)
    cols = len(matrix[0])
    A,B = 0,0
    while A < rows and B < cols-1:
        matrix = reduce_rows(matrix, A, B, modulus)
        if matrix[A][B] != 0:
            A += 1
            B += 1
        else:
            B += 1
    matrix = delete_zero_rows(matrix)
    return matrix

def modrref(matrix, modulus):
    rows = len(matrix)
    cols = len(matrix[0])
    A,B = rows-1,cols-2
    while A >= 0 and B >= 0:
        matrix = upper_triangle(matrix, A, B, modulus)
        if matrix[A][B] != 0:
            A -= 1
            B -= 1
        else:
            B -= 1
    return matrix

def find_relations(root, prime, primes, numRelations):    
    x = 100 # use a larger power to avoid small factorizations
    
    relations = [None] * numRelations
    found = 0
    while found < numRelations:
        n = modexp(root, x, prime)
        row, remainder = factor_from_primes(n, primes)
            
        if remainder == 1:
            row.append(x)
            relations[found] = row
            found += 1
        x += 1
    return relations

@timed_function
def index_calculator_timed(number, root, prime, primeBound, numRelations):
    index_calculator(number, root, prime, primeBound, numRelations)

def index_calculator(number, root, prime, primeBound, numRelations):
    print(f"a = {number}, r = {root}, p = {prime}\n")

    primes = prime_base(primeBound)
    numPrimes = len(primes)

    print(f"Prime base ({numPrimes} primes):\n{primes}\n")

    relations = find_relations(root, prime, primes, numRelations)
    print(f"{numRelations} relations:") 
    format_table(relations)
        
    relations = modref(relations, prime-1)
    format_table(relations)

    relations = modrref(relations, prime-1)
    format_table(relations) # we don't have to print the final rref, just the solutions

    # calculate last relation
    k = 1
    row = []
    while True:
        n = (number * modexp(root, k, prime)) % prime
        row = [0] * numPrimes
        for idx, p in enumerate(primes):
            if (n % p != 0): continue
            
            i = 1 # divide out factor^i from n
            while (n % p**(i+1) == 0):
                i += 1
            n //= p**i
            row[idx] = i
            
        if n == 1:
            #row.append(-k)
            break
        k += 1
    print(f"Special relation: {*row,-k}")
    sum = -k
    for idx, e in enumerate(row):
        if idx >= len(relations):
            raise ArithmeticError('reduce_rows removed too many rows, try increasing numRelations and re-running...')
        sum += relations[idx][numPrimes] * e
    index = sum % (prime-1)
    print(f"I({number}) = {index}")
    
    print(f"modexp({root}, {index}, {prime}) = {modexp(root, index, prime)}")

    return None