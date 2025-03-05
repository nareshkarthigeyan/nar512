def nPi(n):
    piDigits = "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316271201909145648566923460348610454326648213393607260249141273"
    return str(format(int(piDigits[n:n+8]) % 256, '08b'));

def left_rotate(x, n, w=8):
    n = n % w
    return ((x << n) | (x >> (w - n))) & ((1 << w) - 1)  # Mask to w bits

def right_rotate(x, n, w=8):
    n = n % w
    return ((x >> n) | (x << (w - n))) & ((1 << w) - 1)  # Mask to w

def sigma(input):
    num = int(input, 2)
    piVal = int(nPi(num), 2) 
    a = num ^ piVal
    b = right_rotate(num, 7, 8) ^ left_rotate(num, 3, 8)
    c = (num + piVal) % 256
    result = a ^ b ^ c 
    return format(result % 256, '08b')

def bigM(input):
    num = int(input, 2)
    piVal = int(nPi(num), 2)

    a = right_rotate(num, 23, 8)
    b = left_rotate(num, 11, 8)
    c = (num * piVal) % 256
    res = a ^ b ^ c
    return format(res % 256, '08b')

def padding(input):
    length = len(input)
    res = ''.join(format(ord(i), '08b') for i in input)
    res += "1"
    while len(res) % 512 != 448:
        res += "0"
    res += format(length, '064b')
    return res

def compartmentalize(input):
    res = []
    for i in range(0, len(input), 8):
        res.append(input[i:i+8]);
    return res

def salting(input):
    res = []
    for i in range(0, len(input)):
        val = int(sigma(input[i % len(input)]), 2) 
        val ^= int(bigM(input[(i + 8) % len(input)]), 2)
        val = left_rotate(val + i * 17, i % 8, 8)
        res.append(format(val, '08b'))
    return res

def mixing(input):
    res = []
def toHash(input):
    hash = ''
    for i in input:
        num = int(i, 2);
        hash += format(num, '0x')
    return hash

def nar512(input):
    padded = padding(input)
    parts = compartmentalize(padded)
    # print(parts)
    salted = salting(parts)
    # print(salted)
    # print(toHash(parts))
    print(toHash(salted))

nar512("abcd")