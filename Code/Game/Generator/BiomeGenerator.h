#pragma once

enum ContinentalnessType
{
    CONT_DEEP_OCEAN,     // [-1.20, -0.455)
    CONT_OCEAN,          // [-0.455, -0.19)
    CONT_COAST,          // [-0.19, -0.11)
    CONT_NEAR_INLAND,    // [-0.11, 0.03)
    CONT_MID_INLAND,     // [0.03, 0.30)
    CONT_FAR_INLAND      // [0.30, 1.00]
};

enum ErosionType
{
    E0, E1, E2, E3, E4, E5, E6
};

enum PVType
{
    PV_VALLEYS,  // [-1.00, -0.85)
    PV_LOW,      // [-0.85, -0.2)
    PV_MID,      // [-0.2, 0.2)
    PV_HIGH,     // [0.2, 0.7)
    PV_PEAKS     // [0.7, 1.0]
};

enum TemperatureType
{
    T0, T1, T2, T3, T4
};

enum HumidityType
{
    H0, H1, H2, H3, H4
};

class BiomeGenerator
{
public:
    enum BiomeType
    {
        BIOME_PLAINS = 0,      
        BIOME_DESERT = 1,      
        BIOME_FOREST = 2,      
        BIOME_SNOW = 3,
        BIOME_SNOWY_PLAINS = 4,
        BIOME_SNOWY_TAIGA = 5,
        BIOME_SNOWY_BEACH = 6,
        BIOME_DESERT_BEACH = 7,
        BIOME_BEACH = 8,
        BIOME_SNOWY_PEAKS = 9,
        BIOME_SWAMP = 10,
        BIOME_TAIGA = 11,
        BIOME_OCEAN = 12,       
        BIOME_FROZEN_OCEAN = 13,
        BIOME_DEEP_OCEAN = 14,
        BIOME_SAVANNA = 15,
        BIOME_JUNGLE = 16,
        BIOME_UNKNOWN
    };

    struct BiomeParameters
    {
        float m_temperature;       
        float m_humidity;          
        float m_continentalness;   
        float m_erosion;           
        float m_weirdness;         
        float m_peaksAndValleys;   
    };

    BiomeGenerator(unsigned int baseSeed);
    ~BiomeGenerator();
    
    ContinentalnessType ClassifyContinentalness(float c);
    ErosionType ClassifyErosion(float e);
    PVType ClassifyPV(float pv);
    TemperatureType ClassifyTemperature(float t);
    HumidityType ClassifyHumidity(float h);
    
    BiomeType GetMiddleBiome(TemperatureType t, HumidityType h);
    BiomeType GetBeachBiome(TemperatureType t);
    BiomeType GetBadlandBiome(HumidityType h);
    BiomeType DetermineInlandBiome(ContinentalnessType cont, PVType pv, ErosionType erosion,
                                 TemperatureType temp, HumidityType humid);
    BiomeType DetermineBiome(const BiomeParameters& params);
    
    BiomeParameters SampleBiomeParameters(int worldX, int worldY);
    
    BiomeParameters m_biomeParameters;
    unsigned int m_temperatureSeed;
    unsigned int m_humiditySeed;
    unsigned int m_continentalSeed;
    unsigned int m_erosionSeed;
    unsigned int m_weirdnessSeed;
    unsigned int m_peaksValleysSeed;
};
