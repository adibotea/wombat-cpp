/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IterLimDev.h
 * Author: adi
 *
 * Created on 03 January 2018, 14:01
 */

#ifndef ITERLIMDEV_H
#define ITERLIMDEV_H

#include "node.h"

class LimitedDiscrSearch {
public:
    LimitedDiscrSearch(int mb);
    virtual ~LimitedDiscrSearch();
    void run(Node &root);

private:
    int m_maxBudget;
    Node m_lastProcessedNode;

};

#endif /* ITERLIMDEV_H */

