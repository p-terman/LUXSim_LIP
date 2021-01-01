/*

 ***** LUXSimMatlabReader_LoadFile.c *****
 
 Loads data into 1x1 structure with array fields for LUXSimMatlabReader.
 
 2012-11-28 DCM - Adapted from LUXSimMatlabReader_LoadFile.c.
 2012-12-03 DCM - Continually amazed at how many bugs there are. Fixed num_thermalelectrons output
 2013-04-26 DCM - Added event number for primary particles
 2013-04-29 DCM - Bug fix -- step_time now loads
 2013-05-21 DCM - Updated to accommodate output update (now includes primary time)
 2013-06-12 DCM - Had to re-commit step_time bug fix from 2013-04-29, no idea why
 2014-09-30 PBras - Commented primary_time[pri_ctr] to load the time array properly

*/

#include "matrix.h"
#include "mex.h"
#include <string.h>

/*#define DEBUG 1*/

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	/* Open log file for debugging */
	#ifdef DEBUG
	FILE * log_fid = fopen("LUXSimMatlabReader_LoadFile_out.log", "w");
	fprintf(log_fid, "Log file opened\n");
	#endif
	
	/* Get C string of file path / name */
    int file_n_len = mxGetN(prhs[0])+1;
    char * file_n_str = (char *)mxCalloc(file_n_len, sizeof(char)); /* This is the C string to use */
    mxGetString(mxDuplicateArray(prhs[0]), file_n_str, file_n_len);
    
    /* ***** Open file ***** */
    FILE * fid = fopen(file_n_str, "rb");
    
    /* Quit if unsuccessful */
    if (fid == NULL)
    {
    	#ifdef DEBUG
    	fprintf(log_fid, "File %s not found\n", file_n_str);
    	fclose(log_fid);
    	#endif
    	
    	if (nlhs>0)
			plhs[0] = mxCreateDoubleMatrix(0,1,mxREAL);
		if (nlhs>1)
			plhs[1] = mxCreateDoubleMatrix(0,1,mxREAL);
		
    	return;
	}
	
	#ifdef DEBUG
	fprintf(log_fid, "Opened file %s\n", file_n_str);
	#endif
	
	/* ******************************************************************** */
    /* *************************** FILE READING *************************** */
    /* ******************************************************************** */
    
    /* ***** Read preamble ***** */
    
    /* Number of records */
    int num_records;
    fread((char *)(&num_records), sizeof(int), 1, fid);
    #ifdef DEBUG
    fprintf(log_fid, "Number of records: %i\n", num_records);
    #endif
    
    /* Production time string length */
    int prod_t_len;
    fread((char *)(&prod_t_len), sizeof(int), 1, fid);
    #ifdef DEBUG
    fprintf(log_fid, "Production time string length: %i\n", prod_t_len);
    #endif
    /* Production time string */
    char * prod_t = (char *)mxCalloc(prod_t_len+1, sizeof(char));
    fread(prod_t, sizeof(char), prod_t_len, fid);
    prod_t[prod_t_len] = '\0';
    #ifdef DEBUG
    fprintf(log_fid, "Production time string: %s\n", prod_t);
    #endif
    
    /* Geant4 version string length */
    int g4_ver_len;
    fread((char *)(&g4_ver_len), sizeof(int), 1, fid);
    /* Geant4 version string */
    char * g4_ver = (char *)mxCalloc(g4_ver_len+1, sizeof(char));
    fread(g4_ver, sizeof(char), g4_ver_len, fid);
    g4_ver[g4_ver_len] = '\0';
    
    /* Revision number string length */
    int rev_num_len;
    fread((char *)(&rev_num_len), sizeof(int), 1, fid);
    /* Revision number string */
    char * rev_num = (char *)mxCalloc(rev_num_len+1, sizeof(char));
    fread(rev_num, sizeof(char), rev_num_len, fid);
    rev_num[rev_num_len] = '\0';
    
    /* Computer name string length */
    int comp_name_len;
    fread((char *)(&comp_name_len), sizeof(int), 1, fid);
    /* Computer name string */
    char * comp_name = (char *)mxCalloc(comp_name_len+1, sizeof(char));
    fread(comp_name, sizeof(char), comp_name_len, fid);
    comp_name[comp_name_len] = '\0';
    
    /* First-time run stuff */
    
    char *cmds, *svn_diff, *det_comp;
    
    if (!feof(fid))
    {
    	/* Commands string length */
		int cmds_len;
		fread((char *)(&cmds_len), sizeof(int), 1, fid);
		/* Commands string */
		cmds = (char *)mxCalloc(cmds_len+1, sizeof(char));
		fread(cmds, sizeof(char), cmds_len, fid);
		cmds[cmds_len] = '\0';
		
		/* SVN diff string length */
		int svn_diff_len;
		fread((char *)(&svn_diff_len), sizeof(int), 1, fid);
		/* SVN diff string */
		svn_diff = (char *)mxCalloc(svn_diff_len+1, sizeof(char));
		fread(svn_diff, sizeof(char), svn_diff_len, fid);
		svn_diff[svn_diff_len] = '\0';
		
		/* Detector components string length */
		int det_comp_len;
		fread((char *)(&det_comp_len), sizeof(int), 1, fid);
		/* Commands string */
		det_comp = (char *)mxCalloc(det_comp_len+1, sizeof(char));
		fread(det_comp, sizeof(char), det_comp_len, fid);
		det_comp[det_comp_len] = '\0';
    }
    else
    {
    	cmds = (char *)mxCalloc(1, sizeof(char));
    	svn_diff = (char *)mxCalloc(1, sizeof(char));
    	det_comp = (char *)mxCalloc(1, sizeof(char));
    }
    
    /* ******************************************************************** */
    
    
    /* ******************************************************************** */
    /* ************************* GET RECORD DATA ************************** */
    /* ******************************************************************** */
    
    /* **************** Initialize variables **************** */
    
    int pri_ctr = 0;
    int hit_ctr = 0;
    
    /* Event-level records */
    
    double * record_level = (double *)mxCalloc(num_records, sizeof(double));
    double * opticalphoton_record_level = (double *)mxCalloc(num_records, sizeof(double));
    double * thermalelectron_record_level = (double *)mxCalloc(num_records, sizeof(double));
    double * volume_id = (double *)mxCalloc(num_records, sizeof(double));
    double * event_num = (double *)mxCalloc(num_records, sizeof(double));
    double * total_energy_deposition = (double *)mxCalloc(num_records, sizeof(double));
    double * num_opticalphotons = (double *)mxCalloc(num_records, sizeof(double));
    double * num_thermalelectrons = (double *)mxCalloc(num_records, sizeof(double));
    double * hits_size = (double *)mxCalloc(num_records, sizeof(double));
    double * record_hits_ind = (double *)mxCalloc(num_records, sizeof(double));
    
    /* *** Do practice run-through of file, to get total hits length *** */
    /* Get current position in file */
    fpos_t file_pos;
    fgetpos(fid, &file_pos);
    
    /* Loop over file */
    int pri_len = 0;
    int hit_len = 0;
    int ii_rec_practice;
    int tmp_i  = 0;
    int tmp_i2 = 0;
    int tmp_i3 = 0;
    for (ii_rec_practice = 0; ii_rec_practice < num_records; ii_rec_practice++)
    {
    	/* Get primaries size for this record */
    	int pri_len_tmp=0;
    	fread((char *)(&pri_len_tmp), sizeof(int), 1, fid);
    	pri_len += pri_len_tmp;
    	
    	/* Loop over and skip all primaries */
    	int ii_pri;
    	for (ii_pri = 0; ii_pri < pri_len_tmp; ii_pri++)
    	{
    		tmp_i2=0;
    		fread((char *)(&tmp_i2), sizeof(int), 1, fid);
			fseek(fid, tmp_i2*sizeof(char)+8*sizeof(double), SEEK_CUR);
		}
		
		/* Get record level */
		tmp_i=0;
		fread((char *)(&tmp_i), sizeof(int), 1, fid);
		
		/* Get opticalphoton record level */
		tmp_i2=0;
		fread((char *)(&tmp_i2), sizeof(int), 1, fid);
		
		/* Get thermalelectron record level */
		tmp_i3=0;
		fread((char *)(&tmp_i3), sizeof(int), 1, fid);
		
		/* Skip over remaining record-level entries */
		fseek(fid, 2*sizeof(int), SEEK_CUR);
		
		/* Skip over energy deposition */
		if (tmp_i > 0)
			fseek(fid, sizeof(double), SEEK_CUR);
		
		/* Skip over number of opticalphotons */
		if (tmp_i2 > 0)
			fseek(fid, sizeof(int), SEEK_CUR);
		
		/* Skip over number of thermalelectrons */
		if (tmp_i3 > 0)
			fseek(fid, sizeof(int), SEEK_CUR);
		
		/* Get hits size for this record */
		int hit_len_tmp=0;
		fread((char *)(&hit_len_tmp), sizeof(int), 1, fid);
		hit_len += hit_len_tmp;
		
		/* Loop over and skip all hits */
		int ii_hit;
		for (ii_hit = 0; ii_hit < hit_len_tmp; ii_hit++)
		{
			/* Skip over particle name */
			tmp_i2=0;
			fread((char *)(&tmp_i2), sizeof(int), 1, fid);
			fseek(fid, tmp_i2*sizeof(char), SEEK_CUR);
			/* Skip over process name */
			tmp_i2=0;
			fread((char *)(&tmp_i2), sizeof(int), 1, fid);
			fseek(fid, tmp_i2*sizeof(char), SEEK_CUR);
			/* Skip over rest of hits entry */
			fseek(fid, 4*sizeof(int)+9*sizeof(double), SEEK_CUR);
		}
    }
    
    /* Go back to original position */
    fsetpos(fid, &file_pos);
    
    /* *** END file scanning *** */
    
    #ifdef DEBUG
    fprintf(log_fid, "\nTotal number of primaries for all events: %i\n", pri_len);
    fprintf(log_fid, "Total number of hits for all events: %i\n", hit_len);
    #endif
    
    /* Primary particles */
    mxArray * primary_name = mxCreateCellMatrix(1, pri_len);
    double * primary_energy = (double *)mxCalloc(pri_len, sizeof(double));
    double * primary_time = (double *)mxCalloc(pri_len, sizeof(double));
    double ** primary_position = (double **)mxCalloc(3, sizeof(double *));
    double ** primary_direction = (double **)mxCalloc(3, sizeof(double *));
    double * primary_event = (double *)mxCalloc(pri_len, sizeof(double));
    
    /* Hit-level records */
    
    mxArray * particle_name = mxCreateCellMatrix(1, hit_len);
    mxArray * process_name = mxCreateCellMatrix(1, hit_len);
    double * step_num = (double *)mxCalloc(hit_len, sizeof(double));
    double * particle_id = (double *)mxCalloc(hit_len, sizeof(double));
    double * track_id = (double *)mxCalloc(hit_len, sizeof(double));
    double * parent_id = (double *)mxCalloc(hit_len, sizeof(double));
    double * particle_energy = (double *)mxCalloc(hit_len, sizeof(double));
    double ** particle_direction = (double **)mxCalloc(3, sizeof(double *));
    double * energy_deposition = (double *)mxCalloc(hit_len, sizeof(double));
    double ** position = (double **)mxCalloc(3, sizeof(double *));
    double * step_time = (double *)mxCalloc(hit_len, sizeof(double));
    
    /* Initialize 2-D arrays */
    int ii_array;
    
    primary_position[0] = (double *)mxCalloc(pri_len * 3, sizeof(double));
    primary_direction[0] = (double *)mxCalloc(pri_len * 3, sizeof(double));
    
    for (ii_array = 1; ii_array < 3; ii_array++)
    {
    	primary_position[ii_array] = primary_position[ii_array-1] + pri_len;
    	primary_direction[ii_array] = primary_direction[ii_array-1] + pri_len;
    }
    
    particle_direction[0] = (double *)mxCalloc(hit_len * 3, sizeof(double));
    position[0] = (double *)mxCalloc(hit_len * 3, sizeof(double));
    
    for (ii_array = 1; ii_array < 3; ii_array++)
    {
    	particle_direction[ii_array] = particle_direction[ii_array-1] + hit_len;
    	position[ii_array] = position[ii_array-1] + hit_len;
    }
    
    #ifdef DEBUG
	fprintf(log_fid, "\nVariables initialized\n");
    #endif
    
    /* ****************************************************** */
    
    /* ******************* Loop over file ******************* */
    
    int ii_rec;
    for (ii_rec = 0; ii_rec < num_records; ii_rec++)
    {
    	#ifdef DEBUG
    	fprintf(log_fid, "Filling record %i...\n", ii_rec);
    	#endif
    	
    	/* Primary particle info */
    	int primary_size;
    	fread((char *)(&primary_size), sizeof(int), 1, fid);
    	mwSize primary_ndims[1] = { primary_size };
    	#ifdef DEBUG
    	fprintf(log_fid, "primary array size = %i\n", primary_size);
    	#endif
    	
    	int ii_pri;
    	int pri_ctr_evt = pri_ctr;
    	for (ii_pri = 0; ii_pri < primary_size; ii_pri++)
    	{
    		/* Primary particle name */
    		int pri_name_len;
    		fread((char *)(&pri_name_len), sizeof(int), 1, fid);
    		char * pri_name = (char *)mxCalloc(pri_name_len+1, sizeof(char));
    		fread(pri_name, sizeof(char), pri_name_len, fid);
    		pri_name[pri_name_len] = '\0';
    		mxSetCell(primary_name, pri_ctr, mxCreateString(pri_name));
    		mxFree(pri_name);
    		
    		/* Primary particle energy */
    		double pri_energy;
    		fread((char *)(primary_energy+pri_ctr), sizeof(double), 1, fid);
    		primary_energy[pri_ctr] = pri_energy;
    		
    		/* Primary particle time */
    		double pri_time;
    		fread((char *)(primary_time+pri_ctr), sizeof(double), 1, fid);
		/* Time array was always empty when converting to .mat with MEX. Commenting this line solves the problem */
    		/*primary_time[pri_ctr] = pri_time;*/
    		
    		/* Primary particle position */
    		double pri_pos_x;
    		fread((char *)(primary_position[0]+pri_ctr), sizeof(double), 1, fid);
    		double pri_pos_y;
    		fread((char *)(primary_position[1]+pri_ctr), sizeof(double), 1, fid);
    		double pri_pos_z;
    		fread((char *)(primary_position[2]+pri_ctr), sizeof(double), 1, fid);
    		
    		/* Primary particle direction */
    		double pri_dir_x;
    		fread((char *)(primary_direction[0]+pri_ctr), sizeof(double), 1, fid);
    		double pri_dir_y;
    		fread((char *)(primary_direction[1]+pri_ctr), sizeof(double), 1, fid);
    		double pri_dir_z;
    		fread((char *)(primary_direction[2]+pri_ctr), sizeof(double), 1, fid);
    		
    		++pri_ctr;
    	}
    	
    	/* Record level */
    	int rec_lev;
    	fread((char *)(&rec_lev), sizeof(int), 1, fid);
    	record_level[ii_rec] = (double)rec_lev;
    	#ifdef DEBUG
    	fprintf(log_fid, "Record level = %f\n", record_level[ii_rec]);
    	#endif
    	
    	/* Optical photon record level */
    	int opt_ph_rec_lev;
    	fread((char *)(&opt_ph_rec_lev), sizeof(int), 1, fid);
    	opticalphoton_record_level[ii_rec] = (double)opt_ph_rec_lev;
    	#ifdef DEBUG
    	fprintf(log_fid, "Optical photon record level = %f\n", opticalphoton_record_level[ii_rec]);
    	#endif
    	
    	/* Thermal electron record level */
    	int therm_e_rec_lev;
    	fread((char *)(&therm_e_rec_lev), sizeof(int), 1, fid);
    	thermalelectron_record_level[ii_rec] = (double)therm_e_rec_lev;
    	#ifdef DEBUG
    	fprintf(log_fid, "Thermal electron record level = %f\n", thermalelectron_record_level[ii_rec]);
    	#endif
    	
    	/* Volume ID */
    	int vol_id;
    	fread((char *)(&vol_id), sizeof(int), 1, fid);
    	volume_id[ii_rec] = (double)vol_id;
    	
    	/* Event # */
    	int evt_num;
    	fread((char *)(&evt_num), sizeof(int), 1, fid);
    	event_num[ii_rec] = (double)evt_num;
    	
    	/* Now that we know the event #, record that for the primaries for this event */
    	for (ii_pri = 0; ii_pri < primary_size; ii_pri++)
    	{
    		primary_event[pri_ctr_evt] = (double)evt_num;
    		++pri_ctr_evt;
    	}
    	
    	/* Energy deposition */
    	if (rec_lev > 0)
    		fread((char *)(total_energy_deposition+ii_rec), sizeof(double), 1, fid);
    	
    	/* Number of optical photons */
    	if (opt_ph_rec_lev > 0)
    	{
    		int num_ph;
    		fread((char *)(&num_ph), sizeof(int), 1, fid);
    		num_opticalphotons[ii_rec] = (double)num_ph;
    	}
    	
    	/* Number of thermal electrons */
    	if (therm_e_rec_lev > 0)
    	{
    		int num_te;
    		fread((char *)(&num_te), sizeof(int), 1, fid);
    		num_thermalelectrons[ii_rec] = (double)num_te;
    	}
    	
    	/* Record size */
    	int hit_sz;
    	fread((char *)(&hit_sz), sizeof(int), 1, fid);
    	hits_size[ii_rec] = (double)hit_sz;
    	#ifdef DEBUG
    	fprintf(log_fid, "Hits size = %i\n", hit_sz);
    	#endif
    	
    	/* Hits index */
    	record_hits_ind[ii_rec] = hit_ctr+1;
    	
    	/* Load step variables */
    	if (hit_sz > 0)
    	{
    		int ii_hit;
    		for (ii_hit = 0; ii_hit < hit_sz; ii_hit++)
    		{
    			#ifdef DEBUG
    			fprintf(log_fid, "Step %i\n", ii_hit);
    			#endif
    			
    			/*if (ii_hit > 0)
    			{
					fclose(log_fid);
					return;
				}*/
    			
    			/* Particle name length */
    			int par_name_len;
    			fread((char *)(&par_name_len), sizeof(int), 1, fid);
    			/* Particle name */
    			char * par_name = (char *)mxCalloc(par_name_len+1, sizeof(char));
    			fread(par_name, sizeof(char), par_name_len, fid);
    			par_name[par_name_len] = '\0';
    			mxSetCell(particle_name, hit_ctr, mxCreateString(par_name));
    			mxFree(par_name);
    			
    			/* Process name length */
    			int proc_name_len;
    			fread((char *)(&proc_name_len), sizeof(int), 1, fid);
    			/* Process name */
    			char * proc_name = (char *)mxCalloc(proc_name_len+1, sizeof(char));
    			fread(proc_name, sizeof(char), proc_name_len, fid);
    			proc_name[proc_name_len] = '\0';
    			mxSetCell(process_name, hit_ctr, mxCreateString(proc_name));
    			mxFree(proc_name);
    			
    			/* Step number */
    			int stp_num;
    			fread((char *)(&stp_num), sizeof(int), 1, fid);
    			step_num[hit_ctr] = (double)stp_num;
    			
    			/* Particle ID */
    			int par_id;
    			fread((char *)(&par_id), sizeof(int), 1, fid);
    			particle_id[hit_ctr] = (double)par_id;
    			
    			/* Track ID */
    			int trk_id;
    			fread((char *)(&trk_id), sizeof(int), 1, fid);
    			track_id[hit_ctr] = (double)trk_id;
    			
    			/* Parent track ID */
    			int prnt_id;
    			fread((char *)(&prnt_id), sizeof(int), 1, fid);
    			parent_id[hit_ctr] = (double)prnt_id;
    			
    			/* Particle kinetic energy */
    			fread((char *)(particle_energy+hit_ctr), sizeof(double), 1, fid);
    			
    			/* Particle direction */
    			fread((char *)(particle_direction[0]+hit_ctr), sizeof(double), 1, fid);
    			fread((char *)(particle_direction[1]+hit_ctr), sizeof(double), 1, fid);
    			fread((char *)(particle_direction[2]+hit_ctr), sizeof(double), 1, fid);
    			
    			/* Energy deposition */
    			fread((char *)(energy_deposition+hit_ctr), sizeof(double), 1, fid);
    			
    			/* Particle position */
    			fread((char *)(position[0]+hit_ctr), sizeof(double), 1, fid);
    			fread((char *)(position[1]+hit_ctr), sizeof(double), 1, fid);
    			fread((char *)(position[2]+hit_ctr), sizeof(double), 1, fid);
    			
    			/* Step time */
    			fread((char *)(step_time+hit_ctr), sizeof(double), 1, fid);
    			
    			++hit_ctr;
    		}
    	}
    	
    	#ifdef DEBUG
    	fprintf(log_fid, "Hit counter = %i\n", hit_ctr);
    	#endif
    }
    
    /* ******************************************************************** */
    
    /* ******************************************************************** */
    /* ****************** ASSIGN VARIABLES TO OUTPUT ********************** */
    /* ******************************************************************** */
    
    /* *** Populate first output (record struct) *** */
    if (nlhs > 0)
    {
		/* Initialize record struct array */
		const char * record_field_names[] = { "record_level", /* 0 */
											  "opticalphoton_record_level",  /* 1 */
											  "thermalelectron_record_level",	/* 2 */
											  "volume_id",  /* 3 */
											  "event_num",  /* 4 */
											  "total_energy_deposition",  /* 5 */
											  "num_opticalphotons",  /* 6 */
											  "num_thermalelectrons", /* 7 */
											  "hits_size",  /* 8 */
											  "record_hits_ind", /* 9 */
											  "primary_name",  /* 10 */
											  "primary_energy",  /* 11 */
											  "primary_time",  /* 12 */
											  "primary_position",  /* 13 */
											  "primary_direction",  /* 14 */
											  "primary_event_num",	/* 15 */
											  "particle_name",  /* 16 */
											  "process_name", /* 17 */
											  "step_num",  /* 18 */
											  "particle_id",  /* 19 */
											  "track_id",  /* 20 */
											  "parent_id",  /* 21 */
											  "particle_energy",  /* 22 */
											  "particle_direction",  /* 23 */
											  "energy_deposition",  /* 24 */
											  "position",  /* 25 */
											  "step_time"  /* 26 */};
		plhs[0] = mxCreateStructMatrix(1, 1, 27, record_field_names);
		
		/* **** Assign variables to struct **** */
		
		/* Record-level variables */
		mxSetFieldByNumber(plhs[0], 0, 0, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 0)), record_level, num_records*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 1, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 1)), opticalphoton_record_level, num_records*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 2, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 2)), thermalelectron_record_level, num_records*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 3, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 3)), volume_id, num_records*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 4, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 4)), event_num, num_records*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 5, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 5)), total_energy_deposition, num_records*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 6, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 6)), num_opticalphotons, num_records*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 7, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 7)), num_thermalelectrons, num_records*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 8, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 8)), hits_size, num_records*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 9, mxCreateDoubleMatrix(1, num_records, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 9)), record_hits_ind, num_records*sizeof(double));
		
		/* Primary particle info */
		mxSetFieldByNumber(plhs[0], 0, 10, primary_name);
		mxSetFieldByNumber(plhs[0], 0, 11, mxCreateDoubleMatrix(1, pri_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 11)), primary_energy, pri_ctr*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 12, mxCreateDoubleMatrix(1, pri_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 12)), primary_time, pri_ctr*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 13, mxCreateDoubleMatrix(pri_ctr, 3, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 13)), primary_position[0], pri_ctr*3*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 14, mxCreateDoubleMatrix(pri_ctr, 3, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 14)), primary_direction[0], pri_ctr*3*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 15, mxCreateDoubleMatrix(1, pri_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 15)), primary_event, pri_ctr*sizeof(double));
		
		/* Hits-level variables */
		mxSetFieldByNumber(plhs[0], 0, 16, particle_name);
		mxSetFieldByNumber(plhs[0], 0, 17, process_name);
		mxSetFieldByNumber(plhs[0], 0, 18, mxCreateDoubleMatrix(1, hit_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 18)), step_num, hit_ctr*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 19, mxCreateDoubleMatrix(1, hit_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 19)), particle_id, hit_ctr*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 20, mxCreateDoubleMatrix(1, hit_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 20)), track_id, hit_ctr*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 21, mxCreateDoubleMatrix(1, hit_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 21)), parent_id, hit_ctr*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 22, mxCreateDoubleMatrix(1, hit_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 22)), particle_energy, hit_ctr*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 23, mxCreateDoubleMatrix(hit_ctr, 3, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 23)), particle_direction[0], hit_ctr*3*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 24, mxCreateDoubleMatrix(1, hit_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 24)), energy_deposition, hit_ctr*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 25, mxCreateDoubleMatrix(hit_ctr, 3, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 25)), position[0], hit_ctr*3*sizeof(double));
		mxSetFieldByNumber(plhs[0], 0, 26, mxCreateDoubleMatrix(1, hit_ctr, mxREAL));
		memcpy(mxGetPr(mxGetFieldByNumber(plhs[0], 0, 26)), step_time, hit_ctr*sizeof(double));
	}
	
	/* **** Populate second output (info struct) **** */
	if (nlhs > 1)
	{
		const char * info_field_names[8] = { "production_time", "geant4_version", "revision_number", 
			"computer_name", "commands", "svn_diff", "detector_components", "filename" };
		mwSize dims[1] = { 1 };
		plhs[1] = mxCreateStructArray( 1, dims, 8, info_field_names );
		
		mxSetFieldByNumber( plhs[1], 0, 0, mxCreateString(prod_t) );
		mxSetFieldByNumber( plhs[1], 0, 1, mxCreateString(g4_ver) );
		mxSetFieldByNumber( plhs[1], 0, 2, mxCreateString(rev_num) );
		mxSetFieldByNumber( plhs[1], 0, 3, mxCreateString(comp_name) );
		mxSetFieldByNumber( plhs[1], 0, 4, mxCreateString(cmds) );
		mxSetFieldByNumber( plhs[1], 0, 5, mxCreateString(svn_diff) );
		mxSetFieldByNumber( plhs[1], 0, 6, mxCreateString(det_comp) );
		mxSetFieldByNumber( plhs[1], 0, 7, mxCreateString(file_n_str) );
	}
		
    /* *** Free calloc'd variables *** */
    mxFree(prod_t);
    mxFree(g4_ver);
    mxFree(rev_num);
    mxFree(comp_name);
    mxFree(cmds);
    mxFree(svn_diff);
    mxFree(det_comp);
    mxFree(file_n_str);
    
    mxFree(record_level);
    mxFree(opticalphoton_record_level);
    mxFree(volume_id);
    mxFree(event_num);
    mxFree(total_energy_deposition);
    mxFree(num_opticalphotons);
    mxFree(num_thermalelectrons);
    mxFree(hits_size);
    mxFree(record_hits_ind);
    
    /*mxDestroyArray(primary_name);*/
    mxFree(primary_energy);
    mxFree(primary_time);
    mxFree(primary_position);
    mxFree(primary_position[0]);
    mxFree(primary_direction);
    mxFree(primary_direction[0]);
    mxFree(primary_event);
    
    /*mxDestroyArray(particle_name);*/
    /*mxDestroyArray(process_name);*/
    mxFree(step_num);
    mxFree(particle_id);
    mxFree(track_id);
    mxFree(parent_id);
    mxFree(particle_energy);
    mxFree(particle_direction);
    mxFree(particle_direction[0]);
    mxFree(energy_deposition);
    mxFree(position);
    mxFree(position[0]);
    mxFree(step_time);
    
    /* ******************************************************************** */
    
    
    #ifdef DEBUG
    fprintf(log_fid, "\nDone loading file!\n");
    #endif
	
	#ifdef DEBUG
    fprintf(log_fid, "\nOutput assigned!\n");
    #endif
    
    
    /* ******************************************************************** */
    
    /* ***** Close files ***** */
    fclose(fid);
    #ifdef DEBUG
    fprintf(log_fid, "\nBinary file closed\n");
    fclose(log_fid);
    #endif
    
}


