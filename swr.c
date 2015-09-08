//
//  main.c
//  test
//
//  Created by Shan Xiang on 14-9-17.
//  Copyright (c) 2014年 swordx. All rights reserved.
//
//
// NOTICE:
// To anyone who may be reading this code:
// DON'T EVEN TRY TO UNDERSTAND IT!!! BECAUSE I CAN HARDLY UNDERSTAND THIS SHIT I'VE WRITTEN MYSELF!!!!!!!!!!!!!
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int input(double height[]){
    srand((int)time(NULL));
    
    height[0] = rand() % 21;
    height[1] = rand() % 21;
    height[2] = rand() % 21;
    height[3] = rand() % 21;
    height[4] = rand() % 21;
    height[5] = rand() % 21;
    height[6] = rand() % 21;
    height[7] = rand() % 21;
    
    height[0] = 6;
    height[1] = 6;
    height[2] = 6;
    height[3] = 6;
    height[4] = 6;
    height[5] = 6;
    height[6] = 6;
    height[7] = 6;
    
    return 0;
}

struct Candidate {
    double height[8];
    double sum;
    int cnt;
};

void sort(int order[], int rect[], double height[]){
    for (int i = 0; i < 4; i++){
        order[i] = rect[i];
        for (int j = i; j > 0; j--){
            if (height[order[j]] < height[order[j - 1]]){
                int tmp = order[j];
                order[j] = order[j - 1];
                order[j - 1] = tmp;
            }
        }
    }
}

double min(double a, double b){
    if (a < b) return a;
    else return b;
}

double max(double a, double b){
    if (a > b) return a;
    else return b;
}

int check(double height[], double result[], int bigRect[], int leftRect[], int rightRect[]){
    int cnt = 0;
    int orderOfBigRect[4];
    int orderOfLeftRect[4];
    int orderOfRightRect[4];
    sort(orderOfBigRect, bigRect, height);
    sort(orderOfLeftRect, leftRect, height);
    sort(orderOfRightRect, rightRect, height);
    
    
    for (int i = 0; i < 8; i++)
        result[i] = height[i];
    
    if (height[orderOfBigRect[3]] - height[orderOfBigRect[0]] > 6){
        struct Candidate candidate[16];
        for (int j = 0; j < 16; j++){
            for (int k = 0; k < 8; k++)
                candidate[j].height[k] = height[k];
            candidate[j].cnt = 0;
            candidate[j].sum = 0;
        }
        
        for (int j = 0; j < 8; j++){
            int index = j * 2;
            for (int k = 0; k < 4; k++) {
                if (bigRect[k] == j)continue;
                candidate[index].height[bigRect[k]] = max(candidate[index].height[bigRect[k]], candidate[index].height[j]-6);
                candidate[index].height[bigRect[k]] = min(candidate[index].height[bigRect[k]], candidate[index].height[j]);
            }
            index = j * 2 + 1;
            for (int k = 0; k < 4; k++) {
                if (bigRect[k] == j)continue;
                candidate[index].height[bigRect[k]] = max(candidate[index].height[bigRect[k]], candidate[index].height[j]);
                candidate[index].height[bigRect[k]] = min(candidate[index].height[bigRect[k]], candidate[index].height[j]+6);
            }
        }
        for (int j = 0; j < 16; j++) {
            sort(orderOfBigRect, bigRect, candidate[j].height);
            sort(orderOfLeftRect, leftRect, candidate[j].height);
            sort(orderOfRightRect, rightRect, candidate[j].height);
            
            if (candidate[j].height[orderOfLeftRect[3]] - candidate[j].height[orderOfLeftRect[0]] > 6){
                if (orderOfLeftRect[0] + orderOfLeftRect[1] == 1){  //nodes[0] and nodes[1] are the smallest
                    if (candidate[j].height[orderOfLeftRect[3]] - candidate[j].height[orderOfLeftRect[1]] <= 6){
                        candidate[j].height[orderOfLeftRect[0]] = candidate[j].height[orderOfLeftRect[3]] - 6;
                    }
                    else{
                        for (int i = 2; i <= 3; i++)
                            if (candidate[j].height[i] > min(candidate[j].height[0], candidate[j].height[1]) + 6){
                                candidate[j].height[i] = min(candidate[j].height[0], candidate[j].height[1]) + 6;
                            }
                    }
                }
                
                else if (orderOfLeftRect[2] + orderOfLeftRect[3] == 1){  //nodes[0] and nodes[1] are the biggest
                    if (candidate[j].height[orderOfLeftRect[2]] - candidate[j].height[orderOfLeftRect[0]] <= 6){
                        candidate[j].height[orderOfLeftRect[3]] = candidate[j].height[orderOfLeftRect[0]] + 6;
                    }
                    else{
                        for (int i = 2; i <= 3; i++)
                            if (candidate[j].height[i] < max(candidate[j].height[0], height[1]) - 6){
                                candidate[j].height[i] = max(height[0], candidate[j].height[1]) - 6;
                            }
                    }
                }
                
                else{
                    if (candidate[j].height[orderOfLeftRect[2]] - candidate[j].height[orderOfLeftRect[0]] <= 6){
                        candidate[j].height[orderOfLeftRect[3]] = candidate[j].height[orderOfLeftRect[0]] + 6;
                    }
                    else if (candidate[j].height[orderOfLeftRect[3]] - candidate[j].height[orderOfLeftRect[1]] <= 6){
                        candidate[j].height[orderOfLeftRect[0]] = candidate[j].height[orderOfLeftRect[3]] - 6;
                    }
                    else {
                        candidate[j].height[orderOfLeftRect[3]] = max(candidate[j].height[0], candidate[j].height[1]);
                        candidate[j].height[orderOfLeftRect[0]] = min(candidate[j].height[0], candidate[j].height[1]);
                    }
                }
            }
            
            if (candidate[j].height[orderOfRightRect[3]] - candidate[j].height[orderOfRightRect[0]] > 6){
                if (orderOfRightRect[0] + orderOfRightRect[1] == 13){
                    if (candidate[j].height[orderOfRightRect[3]] - candidate[j].height[orderOfRightRect[1]] <= 6){
                        candidate[j].height[orderOfRightRect[0]] = candidate[j].height[orderOfRightRect[3]] - 6;
                    }
                    else{
                        for (int i = 4; i <= 5; i++)
                            if (candidate[j].height[i] > min(candidate[j].height[6], candidate[j].height[7]) + 6){
                                candidate[j].height[i] = min(candidate[j].height[6], candidate[j].height[7]) + 6;
                            }
                    }
                }
                
                else if (orderOfRightRect[2] + orderOfRightRect[3] == 13){
                    if (candidate[j].height[orderOfRightRect[2]] - candidate[j].height[orderOfRightRect[0]] <= 6){
                        candidate[j].height[orderOfRightRect[3]] = candidate[j].height[orderOfRightRect[0]] + 6;
                    }
                    else{
                        for (int i = 4; i <= 5; i++)
                            if (candidate[j].height[i] < max(candidate[j].height[6], candidate[j].height[7]) - 6){
                                candidate[j].height[i] = max(candidate[j].height[6], candidate[j].height[7]) - 6;
                            }
                    }
                }
                
                else{
                    if (candidate[j].height[orderOfRightRect[2]] - candidate[j].height[orderOfRightRect[0]] <= 6){
                        candidate[j].height[orderOfRightRect[3]] = candidate[j].height[orderOfRightRect[0]] + 6;
                    }
                    else if (candidate[j].height[orderOfRightRect[3]] - candidate[j].height[orderOfRightRect[1]] <= 6){
                        candidate[j].height[orderOfRightRect[0]] = candidate[j].height[orderOfRightRect[3]] - 6;
                    }
                    else {
                        candidate[j].height[orderOfRightRect[3]] = max(candidate[j].height[6], candidate[j].height[7]);
                        candidate[j].height[orderOfRightRect[0]] = min(candidate[j].height[6], candidate[j].height[7]);
                    }
                }
            }
            
        }
        
        int min_cnt = 8;
        int min_cnt_index = 0;
        for (int i = 0; i < 16; i++){
            /*printf("%lf\t%lf\t%lf\t%lf\t\n", candidate[i].height[0], candidate[i].height[2], candidate[i].height[4], candidate[i].height[6]);
             printf("%lf\t%lf\t%lf\t%lf\t\n", candidate[i].height[1], candidate[i].height[3], candidate[i].height[5], candidate[i].height[7]);
             */
            
            for (int j = 0; j < 8; j++){
                if (candidate[i].height[j] != height[j])
                    candidate[i].cnt++;
                candidate[i].sum += abs(candidate[i].height[j] - height[j]);
            }
            if (candidate[i].cnt < min_cnt){
                min_cnt = candidate[i].cnt;
                min_cnt_index = i;
            }
            if (min_cnt == candidate[i].cnt && candidate[i].sum < candidate[min_cnt_index].sum)
                min_cnt_index = i;
            
            //printf("cnt:%d sum:%lf\n\n", candidate[i].cnt, candidate[i].sum);
        }
        for (int i = 0; i < 8; i++){
            result[i] = candidate[min_cnt_index].height[i];
        }
        return min_cnt;
        
    }
    
    
    
    
    
    
    else {
        if (height[orderOfLeftRect[3]] - height[orderOfLeftRect[0]] > 6){
            if (orderOfLeftRect[0] + orderOfLeftRect[1] == 1){  //nodes[0] and nodes[1] are the smallest
                if (result[orderOfLeftRect[3]] - result[orderOfLeftRect[1]] <= 6){
                    result[orderOfLeftRect[0]] = result[orderOfLeftRect[3]] - 6;
                    cnt++;
                }
                else{
                    for (int i = 2; i <= 3; i++)
                        if (height[i] > min(height[0], height[1]) + 6){
                            result[i] = min(height[0], height[1]) + 6;
                            cnt++;
                        }
                }
            }
            
            else if (orderOfLeftRect[2] + orderOfLeftRect[3] == 1){  //nodes[0] and nodes[1] are the biggest
                if (result[orderOfLeftRect[2]] - result[orderOfLeftRect[0]] <= 6){
                    result[orderOfLeftRect[3]] = result[orderOfLeftRect[0]] + 6;
                    cnt++;
                }
                else{
                    for (int i = 2; i <= 3; i++)
                        if (height[i] < max(height[0], height[1]) - 6){
                            result[i] = max(height[0], height[1]) - 6;
                            cnt++;
                        }
                }
            }
            
            else{
                if (result[orderOfLeftRect[2]] - result[orderOfLeftRect[0]] <= 6){
                    result[orderOfLeftRect[3]] = result[orderOfLeftRect[0]] + 6;
                    cnt++;
                }
                else if (result[orderOfLeftRect[3]] - result[orderOfLeftRect[1]] <= 6){
                    result[orderOfLeftRect[0]] = result[orderOfLeftRect[3]] - 6;
                    cnt++;
                }
                else {
                    result[orderOfLeftRect[3]] = max(height[0], height[1]);
                    result[orderOfLeftRect[0]] = min(height[0], height[1]);
                    cnt += 2;
                }
            }
        }
        
        if (height[orderOfRightRect[3]] - height[orderOfRightRect[0]] > 6){
            if (orderOfRightRect[0] + orderOfRightRect[1] == 13){
                if (result[orderOfRightRect[3]] - result[orderOfRightRect[1]] <= 6){
                    result[orderOfRightRect[0]] = result[orderOfRightRect[3]] - 6;
                    cnt++;
                }
                else{
                    for (int i = 4; i <= 5; i++)
                        if (height[i] > min(height[6], height[7]) + 6){
                            result[i] = min(height[6], height[7]) + 6;
                            cnt++;
                        }
                }
            }
            
            else if (orderOfRightRect[2] + orderOfRightRect[3] == 13){
                if (result[orderOfRightRect[2]] - result[orderOfRightRect[0]] <= 6){
                    result[orderOfRightRect[3]] = result[orderOfRightRect[0]] + 6;
                    cnt++;
                }
                else{
                    for (int i = 4; i <= 5; i++)
                        if (height[i] < max(height[6], height[7]) - 6){
                            result[i] = max(height[6], height[7]) - 6;
                            cnt++;
                        }
                }
            }
            
            else{
                if (result[orderOfRightRect[2]] - result[orderOfRightRect[0]] <= 6){
                    result[orderOfRightRect[3]] = result[orderOfRightRect[0]] + 6;
                    cnt++;
                }
                else if (result[orderOfRightRect[3]] - result[orderOfRightRect[1]] <= 6){
                    result[orderOfRightRect[0]] = result[orderOfRightRect[3]] - 6;
                    cnt++;
                }
                else {
                    result[orderOfRightRect[3]] = max(height[6], height[7]);
                    result[orderOfRightRect[0]] = min(height[6], height[7]);
                    cnt += 2;
                }
            }
        }
    }
    
    return cnt;
}


int main(int argc, const char * argv[])
{
    double height[8];
    input(height);
    
    printf("%lf\t%lf\t%lf\t%lf\t\n", height[0], height[2], height[4], height[6]);
    printf("%lf\t%lf\t%lf\t%lf\t\n", height[1], height[3], height[5], height[7]);
    
    int bigRect[4] = {0, 1, 6, 7};
    int leftRect[4] = {0, 1, 2, 3};
    int rightRect[4] = {4, 5, 6, 7};
    double result[8];
    int cnt = check(height, result, bigRect, leftRect, rightRect);
    
    
    if (cnt == 0)
        printf("OK!\n");
    else {
        printf("The input isn't validated.\n");
        printf("A feasible scheme of adjustion:\n");
        for (int i = 0; i < 8; i++){
            if (height[i] != result[i])
                printf("Index:%d %lf-->%lf\n", i, height[i], result[i]);
        }
        printf("Count: %d\n", cnt);
    }
    
    return 0;
}
