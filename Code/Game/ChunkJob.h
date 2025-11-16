#pragma once
#include "Engine/Job/JobSystem.h"

class Chunk;

class ChunkJob : public Job
{
public:
    ChunkJob(Chunk* chunk, JobType jobType) ;
    virtual void Execute() override = 0;
    virtual void OnComplete() override = 0;
public:
    Chunk* m_chunk = nullptr;
};

class GenerateChunkJob : public ChunkJob
{
public:
    GenerateChunkJob(Chunk* chunk);
    virtual void Execute() override;
    virtual void OnComplete() override;
public:
    //Chunk* m_chunk = nullptr;
};

class LoadChunkJob : public ChunkJob
{
public:
    LoadChunkJob(Chunk* chunk) ;
    virtual void Execute() override;
    virtual void OnComplete() override;
public:
    //Chunk* m_chunk = nullptr;
};

class SaveChunkJob : public ChunkJob
{
public:
    SaveChunkJob(Chunk* chunk);
    virtual void Execute() override;
    virtual void OnComplete() override;
public:
    //Chunk* m_chunk;
};