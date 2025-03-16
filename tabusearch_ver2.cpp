#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <queue>
#include <math.h>
using namespace std;

class KnapsackTabu {
private:
    int N;  // 物品數量
    int W;  // 背包容量
    vector<int> profit;  // 物品價值
    vector<int> weight;  // 物品重量
    vector<bool> currentSolution;  // 當前解
    vector<bool> bestSolution;     // 最佳解
    int bestProfit;               // 最佳價值
    queue<vector<bool>> tabuList; // 禁忌表，用queue實現FIFO
    int tabuListSize;             // 禁忌表大小
    vector<bool> hasselected;     // 記錄已選擇的物品

    // 檢查解是否在禁忌表中
    bool isInTabuList(const vector<bool>& solution) {
        queue<vector<bool>> tempTabuList = tabuList;
        while(!tempTabuList.empty()) {
            if(tempTabuList.front() == solution) {
                return true;
            }
            tempTabuList.pop();
        }
        return false;
    }

    // 隨機變換函式：70%機率翻轉多個位元，30%機率交換兩個位元
    vector<bool> Tweak(const vector<bool>& solution) {
        vector<bool> newSolution = solution;
        int n = solution.size();

        //70%機率翻轉多個位元
        if (rand() % 100 < 70) {
            // 隨機翻轉 5%~15% 的位元，且至少翻轉3個位元
            int flipCount = n * (5 + rand() % 11) /100;
            for (int i = 0; i < std::max(3, flipCount); i++) {
                int idx = rand() % n;
                newSolution[idx] = !newSolution[idx];
            }
        } else {    //30%機率交換兩個位元
            // 隨機選擇一項已選擇的物品並移除
            vector<int> selectedItems;
            for (int i = 0; i < n; i++) {
                if (solution[i]) selectedItems.push_back(i);
            }
            if (!selectedItems.empty()) {
                int removeIdx = selectedItems[rand() % selectedItems.size()];
                newSolution[removeIdx] = false;
            }

            // 隨機選擇一項未選擇的物品並加入
            vector<int> unselectedItems;
            for (int i = 0; i < n; i++) {
                if (!solution[i]) unselectedItems.push_back(i);
            }
            if (!unselectedItems.empty()) {
                int addIdx = unselectedItems[rand() % unselectedItems.size()];
                newSolution[addIdx] = true;
            }
        }
        return newSolution;
    }

public:
    KnapsackTabu(int n, int w) : N(n), W(w) {
        profit.resize(N);
        weight.resize(N);
        currentSolution.resize(N, false);
        bestSolution.resize(N, false);
        hasselected.resize(N, false);
        bestProfit = 0;
        tabuListSize = N*log(N);  // 設定禁忌表長度為NlogN
    }

    void setItems(int w, int p, int index) {
        weight[index] = w;
        profit[index] = p;
    }
    
    // 測試用，將輸入改為橫向方便測試
    /*
    void setItems(int w, int p, int index) {
        if(p==0){
            weight[index] = w;
        }
        else{
            profit[index] = w;
        }
    }*/

    // 添加解到禁忌表
    void addToTabuList(const vector<bool>& solution) {
        if (tabuList.size() >= tabuListSize) {
            tabuList.pop();  // 如果已滿，先移除最舊的解
        }
        tabuList.push(solution);  // 加入新解
    }

    void initial() {
        // 計算每個物品的性價比並存儲索引
        vector<pair<double, int>> valuePerWeight(N);
        for (int i = 0; i < N; i++) {
            valuePerWeight[i] = make_pair((double)profit[i] / weight[i], i);
        }
        
        // 按照性價比降序排序
        sort(valuePerWeight.begin(), valuePerWeight.end(), greater<pair<double, int>>());
        
        int currentWeight = 0;
        // 依照性價比從高到低嘗試放入物品
        for (const auto& item : valuePerWeight) {
            int idx = item.second;
            if (weight[idx] + currentWeight <= W) {
                currentSolution[idx] = true;
                hasselected[idx] = true;
                currentWeight += weight[idx];
            }
        }

        bestSolution = currentSolution;
        bestProfit = calculateProfit(currentSolution);
        
        addToTabuList(currentSolution);
    }

    void solve() {
        //範例第10行
        vector<bool> SR = Tweak(currentSolution);
        //範例第11行
        for(int i=0;i<1000;i++){
            //範例第12行    
            vector<bool> SW = Tweak(currentSolution);
            //範例第13行
            if(!isInTabuList(SW)){
                if((calculateProfit(SW)>calculateProfit(SR))||isInTabuList(SR)){
                    //範例第14行
                    SR = SW;
                }
            }
        }
        //範例第15行
        if(!isInTabuList(SR)){
            currentSolution = SR;
            addToTabuList(currentSolution);
        }
        //範例第18行
        if(calculateProfit(currentSolution)>bestProfit){
            bestSolution = currentSolution;
            bestProfit = calculateProfit(currentSolution);
        }

    }

    //計算當前解的價值
    int calculateProfit(const vector<bool>& solution) {
        int totalWeight = 0;
        int profit = 0;
        for (int i = 0; i < N; i++) {
            if (solution[i]) {
                totalWeight += weight[i];
                profit += this->profit[i];
            }
        }

        // 若總重量超過容量，返回0（視為不合法解）
        return (totalWeight <= W) ? profit : 0;
    }


    // 獲取最佳解的值
    int getBestProfit() const {
        return bestProfit;
    }

    // 獲取最佳解
    vector<bool> getBestSolution() const {
        return bestSolution;
    }
};


int main() {
    try {
        int N, W;
        cout << "請輸入物品數量(N)和背包容量(W): ";
        cin >> N >> W;

        if (cin.fail()) {
            throw runtime_error("輸入格式錯誤");
        }

        KnapsackTabu knapsack(N, W);

        cout << "請輸入每個物品的重量和價值:\n";
        for (int i = 0; i < N; i++) {
            cout << "物品 " << i+1 << ": ";
            int w, p;
            cin >> w >> p;
            if (cin.fail()) {
                throw runtime_error("輸入格式錯誤");
            }
            knapsack.setItems(w, p, i);
        }

        // 測試用，將輸入改為橫向方便測試
        /*int w;
        for(int i=0;i<N;i++){
            cin>>w;
            knapsack.setItems(w, 0, i);
        }
        for(int i=0;i<N;i++){
            cin>>w;
            knapsack.setItems(w, 1, i);
        }*/
        //初始化禁忌表
        knapsack.initial();
        //執行禁忌搜索
        knapsack.solve();



        // 輸出最佳解
        cout << "max profit:" << knapsack.getBestProfit() << endl;
        cout << "solution:";
        vector<bool> best = knapsack.getBestSolution();
        for(int i = 0; i < N; i++) {
            cout << (best[i] ? "1" : "0");
        }
        cout << endl;
        
    } catch (const exception& e) {
        cerr << "錯誤: " << e.what() << endl;
        return 1;
    }
    //system("pause");
    return 0;

}
