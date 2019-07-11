// Fill out your copyright notice in the Description page of Project Settings.

#include "Question.h"

Question::Question()
{

}

Question::~Question()
{
}

void Question::SetTimesAnsweredCorrectly(int val)
{
	timesAnsweredCorrectly = val;
}


void Question::SetJustAnsweredCorrectly(bool val)
{
	justAnsweredCorrectly = val;
}