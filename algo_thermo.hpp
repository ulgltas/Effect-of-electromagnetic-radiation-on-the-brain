#ifndef ALGO_THERMO_HPP
#define ALGO_THERMO_HPP

#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <cmath>
#include <mkl.h>
#include <stdio.h>
#include "vtl.h"
#include "vtlSPoints.h"

#include "CREATE_GEOMETRY/readInputGeometryFile.h"
#include <omp.h>
#include "mpi.h"
#include "dmumps_c.h"

#include "GridCreator_NEW.h"

int get_my_rank();

void ckeck_MUMPS(DMUMPS_STRUC_C &id);

void init_MUMPS(DMUMPS_STRUC_C &id);

void end_MUMPS(DMUMPS_STRUC_C &id);

void convection_brain(
	double *A,
	MUMPS_INT *Indices_line_A,
	MUMPS_INT *Indices_row_A,
	unsigned int *counter_nonvalue_A,
	unsigned int *position_equation,
	unsigned  int parcours,
	double Delta,
	unsigned int N_x,
	unsigned int N_y,
	unsigned int N_z,
	double h,
	double T_infiny,
	double *k,
	double *convection_contribution,
	unsigned int dir,
	unsigned int wichmaterial);

void condition_limit_equation(
	double *B,
	MUMPS_INT *Indices_line_B, 
	MUMPS_INT *Indices_row_B,
	double *A,
	MUMPS_INT *Indices_line_A, 
	MUMPS_INT *Indices_row_A,
	unsigned int *counter_nonvalue_B,
	unsigned int *counter_nonvalue_A,
	unsigned int *position_equation,
	unsigned int parcours,
	unsigned int *Stateofeachface,
	double Delta,
	unsigned int N_x,
	unsigned int N_y,
	unsigned int N_z,
	unsigned int *neighbooroutside,
	double h,
	double T_infiny,
	double *k,
	double *convection_contribution,
	unsigned int wichmaterial);

void Numberofnon_zero_function(
	MKL_INT *numberofnon_nullvalue_A,
	MKL_INT *numberofnon_nullvalue_B,
	unsigned int *Stateofeachface,
	unsigned int N_x,
	unsigned int N_y,
	unsigned int N_z);

void InitializeTemperature(
	double *InitialTemperature,
	unsigned int Number_total,
	unsigned int N_x,
	unsigned int N_y,
	unsigned int N_z,
	double T_infiny, 
	double Delta,
	double *temperature_initial,
	unsigned int *material_at_nodes);

void ReadFile(int Number_total, double *Q,double dt);

void daxpy_call(int Number_total,double *Q,double *BY_results);

void mkl_call(
	int Number_total,
	double *B, 
	int *Indices_line_B, 
	int *Colonn_line_B, 
	int numberofnon_nullvalue_B,
	double *Temperature_before,
	double *BY_results);

void resolve(
	DMUMPS_STRUC_C &id, 
	vtl::SPoints &grid,
	unsigned int Number_total, 
	unsigned int *Stateofeachface, 
	unsigned int N_x, 
	unsigned int N_y, 
	unsigned int N_z, 
	double Delta, 
	double dt,
	double theta,
	double h,
	double T_infiny,
	unsigned int type_simulation_value,
	double *temperature_initial);

int algo_thermo(
	int argc,
	char **argv, 
	GridCreator_NEW & gridElectro);

#endif
