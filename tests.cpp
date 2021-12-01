#include "AVLTree.h"
#include "Player.h"
#include "Group.h"
#include <iostream>

using namespace std;

int main()
{
    AVLTree<int> tr = AVLTree<int>();
    tr.Insert(9);
    tr.Insert(18);
    tr.Insert(7);
    tr.Insert(20);
    tr.Insert(40);
    tr.Insert(4);
    tr.Insert(0);
    tr.Insert(50);
    tr.Insert(45);
    tr.Insert(41);
    tr.Print();

    tr.Remove(40);
    tr.Print();

    tr.Remove(18);
    tr.Print();

    tr.Remove(0);
    tr.Print();

    tr.Remove(7);
    tr.Print();

    tr.Remove(20);
    tr.Print();

    tr.Remove(4);
    tr.Print();

    tr.Remove(45);
    tr.Print();

    tr.Remove(41);
    tr.Print();

    tr.Remove(9);
    tr.Print();

    tr.Remove(50);
    tr.Print();

    cout << "FINISH" << endl;
    return 0;
}