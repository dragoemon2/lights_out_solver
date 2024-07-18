#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <time.h>

// F2上のガウスの消去法
boost::dynamic_bitset<> gaussian_elimination_bit(std::vector<boost::dynamic_bitset<>> array) {
    int n = array.size();

    for(int i = 0; i < n; i++) {
        // pivot選択
        for(int j = i; j < n; j++) {
            if(array[j][n-i]){
                std::swap(array[i], array[j]);
                break;
            }
        }

        // 下への掃き出し
        for (int j = i+1; j < n; j++) {
            if (i != j && array[j][n-i]) {
                array[j] ^= array[i];
            }
        }

        if(i % 100 == 0){
            std::cout << i << "/" << n << std::endl;
        }
    }

    // 上への掃き出し
    for (int i = n-1; i >= 0; i--) {
        for (int j = i-1; j >= 0; j--) {
            if (array[j][n-i]) {
                array[j] ^= array[i];
            }
        }

        if(i % 100 == 0){
            std::cout << i << "/" << n << std::endl;
        }
    }

    boost::dynamic_bitset<> result(n);
    for (int i = 0; i < n; i++) {
        result[i] = array[i][0];
    }

    return result;
}

// bitの立っているビット数が奇数なら1, 偶数なら0を返す
bool norm_bit(boost::dynamic_bitset<> bit) {
    bool result = 0;

    for (int i = 0; i < bit.size(); i++) {
        if (bit[i]) {
            result ^= 1;
        }
    }

    return result;
}

// Pascal's rhombus mod 2 (https://oeis.org/A059318)
std::vector<bool> pascal(int M, int N) {
    int n = 2*N;

    std::vector<bool> vector1(n+1);
    std::vector<bool> vector2(n+1);

    vector2[0] = 1;

    for(int i=2;i<=M+1;i++){
        for(int j=0;j<std::min(n,i);j++){
            vector1[j] = vector2[j] ^ vector2[abs(j-1)] ^ vector2[j+1] ^ vector1[j];  
        }
        if(i % 100 == 0){
            std::cout << i << "/" << M << std::endl;
        }
        std::swap(vector1, vector2);
    }

    return vector2;
}

// 定数項の部分を計算
std::vector<bool> const_mask(int M, int N) {
    std::vector<bool> vector1(N+2);
    std::vector<bool> vector2(N+2);

    // 線型回帰
    for(int i=2;i<=M+1;i++){
        for(int j=1;j<=(N+1)/2;j++){
            vector1[j] = vector2[j] ^ vector2[j-1] ^ vector2[j+1] ^ vector1[j] ^ 1;
            vector1[N+1-j] = vector1[j];
        }
        if(i % 100 == 0){
            std::cout << i << "/" << M << std::endl;
        }
        std::swap(vector1, vector2);
    }

    return vector2;
}


// LIGHTS OUTの解法
std::vector<std::vector<bool>> lights_out(int M, int N){
    clock_t time1 = clock();

    std::vector<bool> pascal_array = pascal(M, N);
    std::vector<bool> const_mask_array = const_mask(M, N);

    std::vector<boost::dynamic_bitset<>> last(N+2, boost::dynamic_bitset<>(N+1));
    for(int j1=1;j1<=N;j1++){
        for(int j2=1;j2<=N;j2++){
            last[j2][j1] = pascal_array[abs(j2-j1)] ^ pascal_array[abs(2*N+2-j1-j2)] ^ pascal_array[j1 + j2];
            last[j2][0] = const_mask_array[j2];
        }
        if(j1 % 100 == 0){
            std::cout << j1 << "/" << N << std::endl;
        }
    }

    // 拡大係数行列の生成
    last.erase(last.begin());
    last.pop_back();

    clock_t time2 = clock();
    std::cout << "1st step: " << (double)(time2 - time1) / CLOCKS_PER_SEC << "s" << std::endl;

    // ガウスの消去法で一番上の行を求める
    boost::dynamic_bitset<> result = gaussian_elimination_bit(last);

    clock_t time3 = clock();
    std::cout << "2nd step: " << (double)(time3 - time2) / CLOCKS_PER_SEC << "s" << std::endl;

    // 結果
    std::vector<std::vector<bool>> result_map(M+2, std::vector<bool>(N+2));

    // 一番上の行
    for(int j=1;j<=N;j++){
        result_map[1][j] = result[j-1];
    }

    // 2行目以降
    for(int i=2;i<=M;i++){
        for(int j=1;j<=N;j++){
            result_map[i][j] = result_map[i-1][j] ^ result_map[i-1][j-1] ^ result_map[i-1][j+1] ^ result_map[i-2][j] ^ 1;
        }
        if(i % 100 == 0){
            std::cout << i << "/" << M << std::endl;
        }
    }

    // 境界を削除
    result_map.erase(result_map.begin());
    result_map.pop_back();
    for (int i = 0; i < M; i++) {
        result_map[i].erase(result_map[i].begin());
        result_map[i].pop_back();
    }

    clock_t time4 = clock();
    std::cout << "3rd step: " << (double)(time4 - time3) / CLOCKS_PER_SEC << "s" << std::endl;
    std::cout << "total time: " << (double)(time4 - time1) / CLOCKS_PER_SEC << "s" << std::endl;

    return result_map;
}


int main() {
    int M, N;

    std::cout << "Width: ";
    std::cin >> M;

    std::cout << "Height: ";
    std::cin >> N;

    std::cout << "M = " << M << ", N = " << N << std::endl;

    std::vector<std::vector<bool>> result = lights_out(M, N);

    // 画像生成
    cv::Mat image(M, N, CV_8UC1);
    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++){
            image.at<uchar>(i, j) = !result[i][j] * 255;
        }
    }

    cv::imwrite(std::string("lights_out_") + std::to_string(M) + "x" + std::to_string(N) + ".png", image);

    std::cout << "showing image..." << std::endl;

    cv::imshow("lights_out", image);
    cv::waitKey(0);
}