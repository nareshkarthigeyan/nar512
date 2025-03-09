import time

def nPi(n):
    piDigits = (
        "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316271201909145648566923460348610454326648213393607260249141273"
    )
    n = n % len(piDigits)
    slice_length = (n % 7) + 4  # Dynamically vary slice length
    return format(int(piDigits[n:n + slice_length]) % 65536, "016b")


def left_rotate(x, n, w=32):
    return ((x << n) | (x >> (w - n))) & ((1 << w) - 1)


def right_rotate(x, n, w=32):
    return ((x >> n) | (x << (w - n))) & ((1 << w) - 1)


def sigma(input):
    num = int(input, 2)
    piVal = int(nPi(num), 2)
    a = right_rotate(num, 19, 32) ^ left_rotate(num, 13, 32)
    b = (num * piVal + 314159) % 4294967295
    c = (num ^ (piVal >> 2)) * 37
    result = a ^ b ^ c
    return format(result % 4294967295, "032b")


def bigM(input):
    num = int(input, 2)
    piVal = int(nPi(num), 2)
    a = right_rotate(num, 23, 32)
    b = left_rotate(num, 11, 32)
    c = (num * piVal) % 4294967295
    res = a ^ b ^ c
    return format(res % 4294967295, "032b")


def padding(input):
    if isinstance(input, bytes):  # If input is bytes, decode it
        input = input.decode('utf-8')

    length = len(input) * 8  # Convert character count to bit length
    res = ''.join(format(ord(i), '08b') for i in input)
    res += "1"
    while len(res) % 512 != 384:
        res += "0"
    res += format(length, '0128b')  # 128-bit length encoding
    return res


def compartmentalize(input):
    return [input[i:i + 32] for i in range(0, len(input), 32)]


def parsing(input):
    res = list(input)

    for i in range(len(input), 64):
        val = int(sigma(input[i % len(input)]), 2)
        val ^= int(bigM(input[(i + 8) % len(input)]), 2)
        val = (val + left_rotate(val + i * 17, i % 32)) % 4294967295
        val = (val + int(res[i - 16], 2)) % 4294967295
        val ^= right_rotate(int(res[i - 2], 2) * 19, i % 32)
        res.append(format(val % 4294967295, "032b"))

    return res

def golder_ratio():
    return (1 + 5 ** 0.5) / 2

def salting(input):
    res = []
    for i in range(len(input)):
        a = int(input[i], 2)
        index = int(input[(i + 1) % len(input)], 2)
        index = int(nPi(index), 2)
        b = int(input[(i + index) % len(input)], 2)
        res.append(format(int(a * golder_ratio() + b), "032b"))
    return res


def compression(input):
    state = [
        0xcbbb9d5d, 0x629a292a, 0x9159015a, 0x152fecd8,
        0x67332667, 0x8eb44a87, 0xdb0c2e0d, 0x47b5481d,
        0xe19b48a8, 0xc5c95a63, 0xa1e38b93, 0x29d070bb,
        0x5c26c926, 0x33a7d98e, 0x4aab5db7, 0x4f7a04dd
    ]

    for i in range(len(input)):
        t1 = (int(input[i], 2) + int(sigma(format(state[4], '032b')), 2) + int(bigM(format(state[6], '032b')), 2) + left_rotate(state[0], 5, 32)) % 4294967295
        t2 = (int(bigM(format(state[9], '032b')), 2) + int(sigma(format(state[11], '032b')), 2) + right_rotate(state[13], 7, 32)) % 4294967295

        new_state = state[1:] + [0]
        new_state[15] = (state[11] + t1) % 4294967295
        new_state[10] = (t1 + t2) % 4294967295
        new_state[7] = (state[3] + t1) % 4294967295
        new_state[0] = (t1 + t2) % 4294967295
        state = new_state

    return [format(x, '08b') for x in state]


def toHash(input):
    hash = ''
    for i in input:
        num = int(i, 2);
        hash += format(num, 'x')
    return hash

def nar512(input):
    padded = padding(input)
    parts = compartmentalize(padded)
    parsed = parsing(parts)
    salted = salting(parsed)
    compressed = compression(salted)
    return toHash(compressed)

str = "abcsdvkjnsdjkvnsd"
start_time = time.time()
string = nar512(str.encode())
end_time = time.time()
print(f"NAR512: {string}")
print(f"Length of hashed value: {len(string)}")
print(f"Time taken for NAR512: {end_time - start_time} seconds")

