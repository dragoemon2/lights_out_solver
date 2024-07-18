import numpy as np
from PIL import Image


def gaussian_elimination_bit(array: np.ndarray) -> int:
    """有限体F2上での掃き出し法を行い、結果を返す。

    Args:
        array (np.ndarray[int]): 行ベクトル(を2進数で表現したもの)のリスト
    
    Returns:
        int: 結果の列ベクトル(を2進数で表現したもの)
        int: ランク
    """
    n = len(array) # 行ベクトルの数

    ker_dim = 0

    # 下への掃き出し
    for i in range(n):
        # find pivot
        for j in range(i, n):
            if array[j] & (1 << (n-i)):
                array[i], array[j] = array[j], array[i]
                break

        else:
            ker_dim += 1

        # eliminate
        for j in range(i+1, n):
            if array[j] & (1 << (n-i)):
                array[j] ^= array[i]


    # 上への掃き出し
    for i in range(n-1, -1, -1):
        for j in range(i-1, -1, -1):
            if array[j] & (1 << (n-i)):
                array[j] ^= array[i]

    result = 0
    for i in range(n):
        result |= (array[i] & 1) << i

    print(f'ker_dim: {ker_dim}')

    return result

@np.vectorize
def norm_bit(x: int) -> int:
    """xの1の個数が奇数なら1、偶数なら0を返す。
    """
    result = 0

    while x != 0:
        result ^= (x & 1)
        x >>= 1

    return result


def pascal(M: int, N: int) -> np.ndarray:
    """mod 2でのpascal rhombus (https://oeis.org/A059318) を返す。
    """
    n = 2*N

    vector1 = np.zeros(n+1, dtype=bool)
    vector2 = np.zeros(n+1, dtype=bool)

    vector2[0] = 1

    for i in range(2, M+2):
        for j in range(min(n,i)):
            vector1[j] = vector2[j] ^ vector2[abs(j-1)] ^ vector2[j+1] ^ vector1[j]
        vector1, vector2 = vector2, vector1

    return vector2


def const_mask(M: int, N: int) -> np.ndarray:
    """定数部分を返す。
    """
    
    vector1 = np.zeros(N+2, dtype=bool)
    vector2 = np.zeros(N+2, dtype=bool)

    for i in range(2, M+2):
        for j in range(1, (N+3)//2):
            vector1[j] = vector2[j] ^ vector2[abs(j-1)] ^ vector2[j+1] ^ vector1[j] ^ 1
            vector1[N+1-j] = vector1[j]

        vector1, vector2 = vector2, vector1

    return vector2


def lights_out(M: int, N: int) -> np.ndarray:
    """Lights Out Puzzleを解く。

    Args:
        M (int): 幅
        N (int): 高さ

    Returns:
        np.ndarray: 解
    """
    pascal_array = pascal(M, N)
    const_array = const_mask(M, N)

    last = np.zeros(N+2, dtype=object)
    for j1 in range(1,N+1):
        for j2 in range(1,N+1):
            last[j2] |= int(pascal_array[abs(j2-j1)] ^ pascal_array[abs(2*N+2-j1-j2)] ^ pascal_array[j1 + j2]) << j1
            last[j2] |= int(const_array[j2])

    # 拡大係数行列
    last = last[1:-1]

    # ガウスの消去法
    result = gaussian_elimination_bit(last)

    # 結果を生成
    result_map = np.zeros((M+2, N+2), dtype=bool)
    for j in range(1, N+1):
        result_map[1][j] = (result >> (j-1)) & 1

    for i in range(2, M+1):
        for j in range(1, N+1):
            result_map[i][j] = result_map[i-1][j] ^ result_map[i-1][j-1] ^ result_map[i-1][j+1] ^ result_map[i-2][j] ^ 1


    return result_map[1:-1, 1:-1]


if __name__ == '__main__':
    M = int(input('Width: '))
    N = int(input('Height: '))

    result = lights_out(M, N)
    image = Image.fromarray((~result).astype(np.uint8) * 255)

    image.save(f'lights_out_{M}x{N}_py.png')
    image.show()

