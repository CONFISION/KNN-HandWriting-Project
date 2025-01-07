**Read this in other languages: [English](README_en.md), [中文](README.md).**

# This is a project based on C++ language that uses the KNN algorithm to realize handwritten digit recognition.

## Somechange in Leatest update

In the latest update,we add an args,it's default value is 0 which means the program will run with the digitset we provide
if you want to use your own digitset,you can change the dirctory:"/cmake-build-debug-visual-studio/testdigits".Or you can
draw a digit to recognize and save it as terget.txt,You just need to change the args to 1 and run the program.

## What is KNN?

KNN (K-Nearest Neighbors) is a type of supervised learning algorithm that can be used for both classification and regres
sion problems. In this project, we will use KNN to classify handwritten digits.

## How does KNN work?

1.For a given unlabeled sample, calculate its distance from all labeled samples in the training set.

2.Select the K nearest neighbors from the training set based on the calculated distance.

3.Assign the label of the majority of the K nearest neighbors to the unlabeled sample.

4.For classification problems, a voting mechanism is used to decide the category of unlabeled samples, that is, the most
common category among K neighbors is selected as the category of the sample.

5.For regression problems, the average value rule is adopted, that is, the average value of K neighbors is selected as t
he regression value of the sample.

## what environment we used?

> BuildTools: MSVC cl.exe for x86_64 cmake
>
> Editor: JetBrains CLion

## What's lib we use?

> EasyX [Offical](https://easyx.cn/)
>
> dirent.h [Offical](https://web.archive.org/web/20170428133315/http://www.softagalleria.net/dirent.php)

## Core algorithms

```C
typedef struct data
{
    int label;
    int feature[MAX_DIGIT];
    struct data *next = NULL;
} DATA;

float Eucliden_Distance(DATA *pdata,DATA *ptrain)
{
    int i = 0;
    float distance = -1;
    while (i < MAX_DIGIT)
    {
        distance += pow(pdata->feature[i] - ptrain->feature[i], 2);
        i++;
    }
    distance = sqrt(distance);

    return distance;
}
```
