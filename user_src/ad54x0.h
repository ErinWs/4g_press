#ifndef AD54X0_H
#define AD54X0_H

typedef struct _AD54X0_COMPONENTS
{
	char desc[32];
	struct _AD54X0_COMPONENTS *this;
	void (*const write_reset_reg)(unsigned int const *const p); // void write_control_reg(unsigned int const *const p)
	void (*const write_control_reg)(unsigned int const *const p);// void write_control_reg(unsigned int const *const p)
	void (*const read_control_reg )(unsigned int *const p);      // void read_control_reg (unsigned int *const p)
	void (*const write_data_reg)(unsigned int const *const p);   // void write_data_reg   (unsigned int const *const p)
	void (*const read_data_reg )(unsigned int *const p);         // void read_data_reg     (unsigned int *const p)
	void (*const read_status_reg )(unsigned int *const p);       // void read_status_reg     (unsigned int *const p)
	
}ad54x0_comps_t;

extern ad54x0_comps_t ad54x0_comps;
#endif