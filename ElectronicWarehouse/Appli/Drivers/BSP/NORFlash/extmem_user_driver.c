#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"
#if EXTMEM_DRIVER_USER == 1
#include "stm32_user_driver_api.h"
#include "stm32_user_driver_type.h"
#include "norflash.h"

extern XSPI_HandleTypeDef hxspi2;

NORFlash_ObjectTypeDef NORFlashObject = {0};

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Init(uint32_t MemoryId, EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;

    if (NORFlash_Init(&NORFlashObject, &hxspi2, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2)) == NORFlash_OK)
    {
        retr = EXTMEM_DRIVER_USER_OK;

        UserObject->MemID = MemoryId;
        UserObject->PtrUserDriver = (void *)(&NORFlashObject);
    }

    return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_DeInit(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;

    if (NORFlash_Deinit((NORFlash_ObjectTypeDef *)(UserObject->PtrUserDriver)) == NORFlash_OK)
    {
        retr = EXTMEM_DRIVER_USER_OK;

        UserObject->PtrUserDriver = NULL;
    }

    return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Read(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject, uint32_t Address, uint8_t *Data, uint32_t Size)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
    (void)*UserObject;
    (void)Address;
    (void)Data;
    (void)Size;
    return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Write(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject, uint32_t Address, const uint8_t *Data, uint32_t Size)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;

    if (NORFlash_Write((NORFlash_ObjectTypeDef *)(UserObject->PtrUserDriver), Address, Data, Size) == NORFlash_OK)
    {
        retr = EXTMEM_DRIVER_USER_OK;
    }

    return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_EraseSector(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject, uint32_t Address, uint32_t Size)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;

    if (NORFlash_EraseSector((NORFlash_ObjectTypeDef *)(UserObject->PtrUserDriver), Address, Size) == NORFlash_OK)
    {
        retr = EXTMEM_DRIVER_USER_OK;
    }

    return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_MassErase(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
    
    if (NORFlash_EraseChip((NORFlash_ObjectTypeDef *)(UserObject->PtrUserDriver)) == NORFlash_OK)
    {
        retr = EXTMEM_DRIVER_USER_OK;
    }

    return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Enable_MemoryMappedMode(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;

    if (NORFlash_EnableMemoryMappedMode((NORFlash_ObjectTypeDef *)(UserObject->PtrUserDriver)) == NORFlash_OK)
    {
        retr = EXTMEM_DRIVER_USER_OK;
    }

    return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Disable_MemoryMappedMode(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;

    if (NORFlash_DisableMemoryMappedMode((NORFlash_ObjectTypeDef *)(UserObject->PtrUserDriver)) == NORFlash_OK)
    {
        retr = EXTMEM_DRIVER_USER_OK;
    }

    return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_GetMapAddress(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject, uint32_t *BaseAddress)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;

    if (NORFlash_GetMemoryMappedAddress((NORFlash_ObjectTypeDef *)(UserObject->PtrUserDriver), BaseAddress) == NORFlash_OK)
    {
        retr = EXTMEM_DRIVER_USER_OK;
    }

    return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_GetInfo(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject, EXTMEM_USER_MemInfoTypeDef *MemInfo)
{
    EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
    (void)*UserObject;
    (void)MemInfo;
    return retr;
}

/**user-begin**/

EXTMEM_DRIVER_USER_ObjectTypeDef ExtMemObject;

#define READ_DATA_SIZE 256
uint8_t read_buffer[READ_DATA_SIZE];

void APP_NorFlash_Read_Test(void)
{
    EXTMEM_DRIVER_USER_StatusTypeDef status;

    /* 1. 初始化 NorFlash (这里的 MemoryId 参数在您的驱动中仅用作赋值，可填0) */
    status = EXTMEM_DRIVER_USER_Init(0, &ExtMemObject);
    if (status != EXTMEM_DRIVER_USER_OK)
    {
        Error_Handler();
    }

    status = EXTMEM_DRIVER_USER_Enable_MemoryMappedMode(&ExtMemObject);
    if (status != EXTMEM_DRIVER_USER_OK)
    {
        Error_Handler(); // 错误处理
    }


}
/**user-end**/
#endif
