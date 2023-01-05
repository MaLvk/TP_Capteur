#include "functions.h"

#define MAX_SIZE_MES 32
#define RESET_MES_MPU 0x80 //la valeur que l’on va écrire pour la configuration du registre
#define ID_REG 0xE0
#define RESET_MES_BMP 0xB6
#define BEST_PLL 0x02
extern uint8_t bcom[MAX_SIZE_MES]; //récupère la valeur écrit dans le registre (vérification)


void Init(I2C_HandleTypeDef* hi2c1){
	  bcom[0]=BEST_PLL;
	  HAL_I2C_Mem_Write(hi2c1, MPU_ADD, PWR_MGMT_1, 1, bcom, 1, HAL_MAX_DELAY);
	  HAL_I2C_Mem_Read(hi2c1, MPU_ADD, PWR_MGMT_1, 1, bcom, 1, HAL_MAX_DELAY);
	  printf("La valeur de la PLL est : 0x%x\r\n", bcom[0]);

	  //reset capteur MPU
	  printf("Starting MPU Reset Hardware: \r\n");
	  bcom[0]=RESET_MES_MPU;
	  HAL_I2C_Mem_Write(hi2c1, MPU_ADD, PWR_MGMT_1, 1, bcom, 1, HAL_MAX_DELAY);
	  HAL_I2C_Mem_Read(hi2c1, MPU_ADD, PWR_MGMT_1, 1, bcom, 1, HAL_MAX_DELAY);
	  printf("La valeur du reset MPU : 0x%x\r\n",bcom[0]);

	  //reset capteur BMP
	  printf("Starting BMP Reset Hardware: \r\n");
	  bcom[0]=RESET_MES_BMP;
	  HAL_I2C_Mem_Write(hi2c1, MPU_ADD, ID_REG, 1, bcom, 1, HAL_MAX_DELAY);
	  HAL_I2C_Mem_Read(hi2c1, MPU_ADD, ID_REG, 1, bcom, 1, HAL_MAX_DELAY);
	  printf("La valeur du reset BMP : 0x%x\r\n",bcom[0]);

	  HAL_Delay(100);

	  //lecture PLL
	  HAL_I2C_Mem_Read(hi2c1, MPU_ADD, PWR_MGMT_1, 1, bcom, 1, HAL_MAX_DELAY);
	  printf("La valeur de la PLL est : 0x%x\r\n", bcom[0]);

	  //choix de la PLL
	  printf("Starting PLL Choice: \r\n");
	  bcom[0]=BEST_PLL;
	  HAL_I2C_Mem_Write(hi2c1, MPU_ADD, PWR_MGMT_1, 1, bcom, 1, HAL_MAX_DELAY);
	  HAL_I2C_Mem_Read(hi2c1, MPU_ADD, PWR_MGMT_1, 1, bcom, 1, HAL_MAX_DELAY);
	  printf("La valeur de la PLL est : 0x%x\r\n", bcom[0]);

}

void Measure_T(I2C_HandleTypeDef* hi2c,double* buffer_temp){
	uint16_t TEMP_OUT;
	uint8_t bTEMP_OUT_H[MAX_SIZE_MES];
	uint8_t bTEMP_OUT_L[MAX_SIZE_MES];

	HAL_I2C_Mem_Read(hi2c, MPU_ADD, TEMP_OUT_H, 1, bTEMP_OUT_H, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c, MPU_ADD, TEMP_OUT_L, 1, bTEMP_OUT_L, 1, HAL_MAX_DELAY);

	TEMP_OUT= (bTEMP_OUT_H[0]<<8)+bTEMP_OUT_L[0];
	buffer_temp[0]=((TEMP_OUT-ROOM_TEMP_OFFSET)/TEMP_SENS)+21;

	printf("La temperature est de : %1.5f\r\n",buffer_temp[0]);
}

void Measure_A(I2C_HandleTypeDef* hi2c,double* buffer_acceleration){
	float axis_x;
	float axis_y;
	float axis_z;
	uint8_t b_OUT_H[MAX_SIZE_MES];
	uint8_t b_OUT_L[MAX_SIZE_MES];


	printf("Starting config accel: \r\n");
	bcom[0]=0x00;
	HAL_I2C_Mem_Write(hi2c, MPU_ADD, ACCEL_CONFIG, 1, bcom, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c, MPU_ADD, ACCEL_CONFIG, 1, bcom, 1, HAL_MAX_DELAY);
	printf("L'accel est config : 0x%x\r\n", bcom[0]);

	//lecture axe X
	HAL_I2C_Mem_Read(hi2c, MPU_ADD, ACCEL_XOUT_H, 1, b_OUT_H, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c, MPU_ADD, ACCEL_XOUT_L, 1, b_OUT_L, 1, HAL_MAX_DELAY);
	axis_x= (b_OUT_H[0]<<8)+b_OUT_L[0];

	//lecture axe Y
	HAL_I2C_Mem_Read(hi2c, MPU_ADD, ACCEL_YOUT_H, 1, b_OUT_H, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c, MPU_ADD, ACCEL_YOUT_L, 1, b_OUT_L, 1, HAL_MAX_DELAY);
	axis_y= (b_OUT_H[0]<<8)+b_OUT_L[0];

	//lecture axe Z
	HAL_I2C_Mem_Read(hi2c, MPU_ADD, ACCEL_ZOUT_H, 1, b_OUT_H, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(hi2c, MPU_ADD, ACCEL_ZOUT_L, 1, b_OUT_L, 1, HAL_MAX_DELAY);
	axis_z= (b_OUT_H[0]<<8)+b_OUT_L[0];

	buffer_acceleration[0]=((axis_x)/32768)-1;
	buffer_acceleration[1]=((axis_y)/32768)-1;
	buffer_acceleration[2]=((axis_z)/32768)-1;

	float G = sqrt(buffer_acceleration[0]*buffer_acceleration[0]+buffer_acceleration[1]*buffer_acceleration[1]+buffer_acceleration[2]*buffer_acceleration[2]);

	printf("L'acceleration sur x : %1.5f\r\n",buffer_acceleration[0]);
	printf("L'acceleration sur y : %1.5f\r\n",buffer_acceleration[1]);
	printf("L'acceleration sur z : %1.5f\r\n",buffer_acceleration[2]);

	printf("La norme du vecteur gravite est : %1.5f\r\n",G);
}
