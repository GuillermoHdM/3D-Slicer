#pragma once
#include <cstdint>
#include <vector>

//this whole file follows the official specifications of Elegoo 1.2: 
//https://github.com/elegooofficial/GOO/blob/main/Goo%20Format%20Spec%20V1.2.pdf

#pragma pack(push, 1)
struct header_info
{
	// INDEX - FIELD - TYPE - LENGTH(byte) - DEFINITIONS

	// 0 - version - String - 4 - format version (constante: "V1.2")
	char version[4] = { 'V', '1', '.', '2' };
	// 1 - Magic Tag - String - 8 - Fix constant: 0x07 0x00 0x00 0x00 0x44 0x4C 0x50 0x00
	char magic_tag[8] = { 0x07, 0x00, 0x00, 0x00, 0x44, 0x4C, 0x50, 0x00 };
	// 2 - Software info - String - 32 - Software info
	char software_info[32];
	// 3 - Software version - String - 24 - Software version
	char software_version[24];
	// 4 - File time - String - 24 - File create time
	char file_time[24];
	// 5 - Printer name - String - 32 - Printer name
	char printer_name[32];
	// 6 - Printer type - String - 32 - Printer type
	char printer_type[32];
	// 7 - Profile name - String - 32 - Resin profile name
	char profile_name[32];
	// 8 - Anti-aliasing level - short int - 2 - Anti-aliasing level setting by slicer
	uint16_t anti_aliasing_level;
	// 9 - Grey level - short int - 2 - Grey level
	uint16_t grey_level;
	// 10 - Blur level - short int - 2 - Blur level
	uint16_t blur_level;
	// 11 - Small Preview Image Data - RGB_565 16Bit - 2*116*116 (26912 bytes)
	uint16_t small_preview[116 * 116];
	// 12 - Delimiter - String - 2 - Fix string: 0xd, 0xa
	char delimiter_1[2] = { 0x0D, 0x0A };
	// 13 - Big preview Image Data - RGB_565 16Bit - 2*290*290 (168200 bytes)
	uint16_t big_preview[290 * 290];
	// 14 - Delimiter - String - 2 - Fix string: 0xd, 0xa
	char delimiter_2[2] = { 0x0D, 0x0A };
	// 15 - Total layers - int - 4 - Total number of layers
	uint32_t total_layers;
	// 16 - X resolution - short int - 2 - Resolution of printing LCD in x direction
	uint16_t x_resolution;
	// 17 - Y resolution - short int - 2 - Resolution of printing LCD in y direction
	uint16_t y_resolution;
	// 18 - X mirror - bool - 1 - 1 is mirror
	uint8_t x_mirror;
	// 19 - Y mirror - bool - 1 - 1 is mirror
	uint8_t y_mirror;
	// 20 - X size of platform - float - 4 - unit: mm (active area x)
	float x_size_platform;
	// 21 - Y size of platform - float - 4 - unit: mm (active area y)
	float y_size_platform;
	// 22 - Z size of platform - float - 4 - unit: mm (active area z)
	float z_size_platform;
	// 23 - Layer thickness - float - 4 - unit: mm
	float layer_thickness;
	// 24 - Common exposure time - float - 4 - Common layer exposure time. unit: s
	float common_exposure_time;
	// 25 - Exposure delay mode - bool - 1 - 1: Static time; 0: Turn Off time
	uint8_t exposure_delay_mode;
	// 26 - Turn off time - float - 4 - Delay time of exposure in Turn-off-time mode. unit: s
	float turn_off_time;
	// 27 - Bottom before lift time - float - 4 - Waiting time before lift for bottom layers. unit: s
	float bottom_before_lift_time;
	// 28 - Bottom after lift time - float - 4 - Waiting time after lift for bottom layers. unit: s
	float bottom_after_lift_time;
	// 29 - Bottom after retract time - float - 4 - Waiting time after retract for bottom layers. unit: s
	float bottom_after_retract_time;
	// 30 - Before lift time - float - 4 - Waiting time before lift for common layers. unit: s
	float before_lift_time;
	// 31 - After lift time - float - 4 - Waiting time after lift for common layers. unit: s
	float after_lift_time;
	// 32 - After retract time - float - 4 - Waiting time after retract for common layers. unit: s
	float after_retract_time;
	// 33 - Bottom exposure time - float - 4 - Exposure time of bottom layer. unit: s
	float bottom_exposure_time;
	// 34 - Bottom layers - int - 4 - The number of bottom layers
	uint32_t bottom_layers;
	// 35 - Bottom lift distance - float - 4 - The lift distance for bottom layers. unit: mm
	float bottom_lift_distance;
	// 36 - Bottom lift speed - float - 4 - Lift speed for bottom layers. unit: mm/min
	float bottom_lift_speed;
	// 37 - Lift distance - float - 4 - Lift distance for common layers. unit: mm
	float lift_distance;
	// 38 - Lift speed - float - 4 - Lift speed for common layers. unit: mm/min
	float lift_speed;
	// 39 - Bottom retract distance - float - 4 - Retract distance for bottom layers. unit: mm
	float bottom_retract_distance;
	// 40 - Bottom retract speed - float - 4 - Retract speed for bottom layers. unit: mm/min
	float bottom_retract_speed;
	// 41 - Retract distance - float - 4 - Retract distance for common layers. unit: mm
	float retract_distance;
	// 42 - Retract speed - float - 4 - Retract speed for common layers. unit: mm/min
	float retract_speed;
	// 43 - Bottom second lift distance - float - 4 - Lift distance of second stage for bottom layers. unit: mm
	float bottom_second_lift_distance;
	// 44 - Bottom second lift speed - float - 4 - Lift speed of second stage for bottom layers. unit: mm/min
	float bottom_second_lift_speed;
	// 45 - Second lift distance - float - 4 - Lift distance of second stage for common layers. unit: mm
	float second_lift_distance;
	// 46 - Second lift speed - float - 4 - Lift speed of second stage for common layers. unit: mm/min
	float second_lift_speed;
	// 47 - Bottom second retract distance - float - 4 - Retract distance of second stage for bottom layers. unit: mm
	float bottom_second_retract_distance;
	// 48 - Bottom second retract speed - float - 4 - Retract speed of second stage for bottom layers. unit: mm/min
	float bottom_second_retract_speed;
	// 49 - Second retract distance - float - 4 - Retract distance of second stage for common layers. unit: mm
	float second_retract_distance;
	// 50 - Second retract speed - float - 4 - Retract speed of second stage for common layers. unit: mm/min
	float second_retract_speed;
	// 51 - Bottom light PWM - short int - 2 - Power of light for bottom layers (0 ~ 255)
	uint16_t bottom_light_pwm;
	// 52 - Light PWM - short int - 2 - Power of light for common layers (0 ~ 255)
	uint16_t light_pwm;
	// 53 - Advance mode - bool - 1 - 0: normal mode; 1: advance mode
	uint8_t advance_mode;
	// 54 - Printing time - int - 4 - The printing times. unit: s
	uint32_t printing_time;
	// 55 - Total volume - float - 4 - Volume of all parts. unit: mm3
	float total_volume;
	// 56 - Total weight - float - 4 - Weight of all parts. unit: g
	float total_weight;
	// 57 - Total price - float - 4 - Cost of all resin used
	float total_price;
	// 58 - Price unit - String - 8 - Unit of price. eg: "$"
	char price_unit[8];
	// 59 - Offset of LayerContent - int - 4 - Position of LayerContent start address (eg: 0x2FAB7)
	uint32_t offset_of_layer_content;
	// 60 - Gray scale level - bool - 1 - 0: range 0x0~0xF; 1: range 0x0~0xFF
	uint8_t gray_scale_level;
	// 61 - Transition layers - short int - 2 - Number of transition layers
	uint16_t transition_layers;
};
struct layer_content
{
	struct layer_definition
	{
		// 1 - Pause flag - short int - 2 bytes
		uint16_t pause_flag = 0;
		// 2 - Pause position Z - float - 4 bytes
		float pause_position_z = 0.0f;
		// 3 - layer position Z - float - 4 bytes
		float layer_position_z = 0.0f;
		// 4 - layer exposure time - float - 4 bytes
		float layer_exposure_time = 0.0f;
		// 5 - layer off time - float - 4 bytes
		float layer_off_time = 0.0f;
		// 6 - Before lift time - float - 4 bytes
		float before_lift_time = 0.0f;
		// 7 - After lift time - float - 4 bytes
		float after_lift_time = 0.0f;
		// 8 - After retract time - float - 4 bytes
		float after_retract_time = 0.0f;
		// 9 - Lift distance - float - 4 bytes
		float lift_distance = 0.0f;
		// 10 - Lift speed - float - 4 bytes
		float lift_speed = 0.0f;
		// 11 - Second lift distance - float - 4 bytes
		float second_lift_distance = 0.0f;
		// 12 - Second lift speed - float - 4 bytes
		float second_lift_speed = 0.0f;
		// 13 - Retract distance - float - 4 bytes
		float retract_distance = 0.0f;
		// 14 - Retract speed - float - 4 bytes
		float retract_speed = 0.0f;
		// 15 - Second retract distance - float - 4 bytes
		float second_retract_distance = 0.0f;
		// 16 - Second retract speed - float - 4 bytes
		float second_retract_speed = 0.0f;
		// 17 - Light PWM - short int - 2 bytes
		uint16_t light_pwm = 255;
		// 18 - Delimiter - String 2 - Fix string: 0xd, 0xa
		char delimiter[2] = { 0x0D, 0x0A };
	} definition;

	// 1 - Data size - int - 4 - The size of encoded image data
	uint32_t layer_data_size = 0;
	std::vector<uint8_t> image_data;
	char delimiter[2] = { 0x0D, 0x0A };
};

#pragma pack(pop)