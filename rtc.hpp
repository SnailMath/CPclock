#include <stdint.h>

//namespace RTC{
	static const unsigned int baseAddress = 0xA413FEC0;
	enum Registers16 {
		RYRCNT	= 0x0E, //Year counter
	};
	enum Registers8 {
		R64CNT	= 0x00, //64Hz counter
		RSECCNT	= 0x02, //Second counter
		RMINCNT	= 0x04, //Minute counter
		RHRCNT	= 0x06, //Hour   counter
		RWKCNT	= 0x08, //Day of week counter
		RDAYCNT	= 0x0A, //Date counter
		RMONCNT	= 0x0C, //Month counter
		RCR2	= 0x1E, //RTC Control Register 2

	};

	inline volatile uint8_t  *regAddress (Registers8 reg) {
		return reinterpret_cast<volatile uint8_t* >(baseAddress +  reg);
	}
	inline volatile uint16_t *regAddress (Registers16 reg) {
		return reinterpret_cast<volatile uint16_t*>(baseAddress +  reg);
	}

	inline uint8_t  regRead(Registers8  reg){
		return *regAddress(reg);
	}
	inline uint16_t regRead(Registers16 reg){
		return *regAddress(reg);
	}

	inline void regWrite(Registers8  reg, uint8_t  value){
		*regAddress(reg) = value;
	}
	inline void regWrite(Registers16 reg, uint16_t value){
		*regAddress(reg) = value;
	}

	enum RCR2_BITS{
		RTC_START	= 0b00000001, //enable to start, stop to change time
		RTC_RESET	= 0b00000010, //reset divider cirquit
		RTC_ROUND	= 0b00000100, //30second adjust
		RTC_HALT	= 0b00001000, //halt crystal oscillator
		RTC_INT0	= 0b00010000, //Timer Interrupt settings
		RTC_INT1	= 0b00100000, //0=disabled, 1=1/256s, 2=1/64s, 3=1/16s
		RTC_INT2	= 0b01000000, //4=1/4s, 5=1/2s, 6=1s, 7=2s
		RTC_INTERRUPT	= 0b10000000  //reset to clear interrupt
	};
	inline bool regTestBit(	RCR2_BITS bit){
		return regRead(	RCR2) & bit;}
	inline void regSetBit(	RCR2_BITS bit){
		regWrite(	RCR2,regRead(
				RCR2)|bit);}
	inline void regClearBit(RCR2_BITS bit){
		regWrite(	RCR2,regRead(
				RCR2)&(~bit));}
//};
