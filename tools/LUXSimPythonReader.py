import os
import time
import numpy as np
from struct import unpack, calcsize

def GetAttribute(file, fmt, length=1):
    if length == 1:
        input_fmt = fmt
        output = unpack(input_fmt, file.read(length*calcsize(fmt)))[0]
    else:
        input_fmt = fmt * length
        temp = unpack(input_fmt, file.read(length*calcsize(fmt)))
        output = ''.join(temp)
    return output

def LUXSimPythonReader(file_directory='.', files=[], save_directory='.'):
    # LUXSimPythonReader(file_directory, files, save_directory, options)
    #
    # Reads LUXSim binary output files and saves the data in a Python struct
    # array in file [BINARY_FILE_NAME].mat.
    #
    # Inputs:
    #  file_directory - path to the directory containing the files to load. If
    #   no argument is specified or if file_directory is an empty array, the
    #   current directory is assumed.
    #  files - file names to load. If there is no second argument
    #   or if files is an empty array, all files are loaded.
    #  save_directory - path to the directory to save the Python files to. If
    #   there is no third argument or if save_directory is an empty array,
    #   files are saved to the binary file directory (file_directory).
    #
    # Outputs:
    #  Data is saved in struct array named 'record.' Please see the LUXSim
    #  TWiki page for Python output format documentation.
    #
    # Changes:
    #  v1.0 KOS 2015-01-25 - Initial submission.
    #       KK  2015-08-24 - Added the step process name parsing

    #% Input handling

    # check to make sure we have read permissions
    if not os.access(file_directory, os.R_OK) or not os.access(file_directory, os.X_OK):
        print('No permission to read directory %s\\n' % file_directory)

    do_file_search = False
    if not files:
        files = os.listdir(file_directory)
        if not files:
            print('No files found in directory')
    else:
        do_file_search = True
        file_list = os.listdir(file_directory)
        files = [file for file in file_list if file in files]

    # check to make sure we have write permissions
    if not os.access(file_directory, os.W_OK) or not os.access(file_directory, os.X_OK):
        print('No permission to write to directory %s\\n' % save_directory)

    continuous = False
    overwrite = False
    save_hit_names = True
    save_primary_names = True

    #% Load files

    function_start_time = time.time()

    flag_run = True
    while flag_run:

        # run endlessly if in continuous mode
        flag_run = continuous

        # if we are in continuous mode, get file list, order by date created, and
        # ignore the last file
        if continuous:

            print('Searching for new files...')

            file_list = os.listdir(file_directory)
            if do_file_search:
                files = [file for file in files if file in file_list]
                file_list = files

            overwrite = False

        for ii_file in range(len(files)):

            file_start_time = time.time()

            filename = files[ii_file]

            # check to see if we are overwriting, and if file already exists
            save_filename = filename.split('.')[0] + '.npz'
            if not overwrite and os.path.isfile(save_filename):
                continue

            print('Loading file %s (%d/%d)') % (filename, ii_file, len(files))
            # Do all file loading / reading operations in LUXSimPythonReader_LoadFile
            full_file_path = '%s/%s' % (file_directory, filename)
            print('Loading file %s') % full_file_path
            record, info = LoadFile(full_file_path, save_hit_names, save_primary_names)
            if not record:
               print('WARNING: no info found for file %s in directory %s') % (filename, file_directory)
               print('Moving on...')
               continue

            file_end_time = time.time()

            # figure out whether to save names
            if not save_primary_names:
                del(record['primary_name'])
            if not save_hit_names:
                del(record['particle_name'])
                del(record['creator_process_name'])
                del(record['step_process_name'])

            save_filename_fullpath = '%s/%s' % (save_directory,  save_filename)
            fsave = open(save_filename_fullpath, 'wb')
            # combine info and record into a single dictionary
            sim_output = {}
            sim_output = record
            np.savez_compressed(fsave, **sim_output)
            fsave.close()

            print('Data saved to %s') % (save_filename_fullpath)

        function_end_time = time.time()

        if continuous:
            wait_time = 60
            print('Checking for new files at %s') % (datestr(now + wait_time / 24 / 60 / 60))
            time.sleep(wait_time)

    print('************************************************************************************')
    print('All files processed (%1.0f s)') % (function_end_time - function_start_time)

def LoadFile(file=None, save_hit_names=None, save_primary_names=None):

    # open file
    f = open(file, 'rb')

    if os.stat(file).st_size == 0:
        print('WARNING: file %s is empty') % (file)
        print('Moving on...')
        return

    #%% Preamble

    info = {}
    record = {}

    # read number of records
    record_length = GetAttribute(f, 'I')

    # read production time
    production_time_length = GetAttribute(f, 'i')
    info['production_time'] = GetAttribute(f, 'c', production_time_length)

    # read Geant4 version
    geant4_version_length = GetAttribute(f, 'i')
    info['geant4_version'] = GetAttribute(f, 'c', geant4_version_length)

    # read revision number
    revision_number_length = GetAttribute(f, 'i')
    info['revision_number'] = GetAttribute(f, 'c', revision_number_length)

    # read computer name
    computer_name_length = GetAttribute(f, 'i')
    info['computer_name'] = GetAttribute(f, 'c', computer_name_length)

    #%% First-time run stuff
    if f.tell() != os.fstat(f.fileno()).st_size:
        # read commands
        commands_length = GetAttribute(f, 'i')
        info['commands'] = GetAttribute(f, 'c', commands_length)

        # read differences
        svn_diff_length = GetAttribute(f, 'i')
        info['svn_diff'] = GetAttribute(f, 'c', svn_diff_length)

        # read detector components
        det_comp_length = GetAttribute(f, 'i')
        info['detector_components'] = GetAttribute(f, 'c', det_comp_length)

    info['filename'] = file


    #%% Particle records

    total_loaded_bytes = 28 + production_time_length + geant4_version_length + revision_number_length + computer_name_length + commands_length + svn_diff_length + det_comp_length
    pri_counter = 0
    step_counter = 0

    record_level = np.zeros(record_length, dtype=np.int)
    opticalphoton_record_level = np.zeros(record_length, dtype=np.int)
    thermalelectron_record_level = np.zeros(record_length, dtype=np.int)
    volume_id = np.zeros(record_length, dtype=np.int)
    event_num = np.zeros(record_length, dtype=np.int)
    total_energy_deposition = np.zeros(record_length)
    num_opticalphotons = np.zeros(record_length, dtype=np.int)
    num_thermalelectrons = np.zeros(record_length, dtype=np.int)
    hits_size = np.zeros(record_length, dtype=np.int)
    record_hits_ind = np.zeros(record_length, dtype=np.int)

    mult_p = 10
    primary_name = np.array([''])
    primary_energy = np.zeros(1)
    primary_time = np.zeros(1)
    primary_position = np.zeros([1, 3])
    primary_direction = np.zeros([1, 3])
    primary_event_num = np.zeros([1, 3], dtype=np.int)

    particle_name = np.array(['']);
    creator_process_name = np.array([''])
    step_process_name = np.array([''])
    step_num = np.zeros(1, dtype=np.int)
    particle_id = np.zeros(1, dtype=np.int)
    track_id = np.zeros(1, dtype=np.int)
    parent_id = np.zeros(1, dtype=np.int)
    particle_energy = np.zeros(1)
    particle_direction = np.zeros([1, 3])
    energy_deposition = np.zeros(1)
    position = np.zeros([1, 3])
    step_time = np.zeros(1)

    update_time = 5#s
    for record_counter in range(record_length):

        if record_counter % 10 == 0:
            print('record = %d / %d') % (record_counter, record_length)
        #%% primary particle info
        # primary particle size
        primary_size = GetAttribute(f, 'i')

        pri_counter_save = pri_counter
        pri_counter_evt = pri_counter
        for ii_pri in range(primary_size):
            if pri_counter == 0:
                # primary particle name
                primary_name_length = GetAttribute(f, 'i')
                name_tmp = GetAttribute(f, 'c', primary_name_length)
                if save_primary_names:
                    primary_name[0] = name_tmp
                # primary particle energy
                primary_energy[0] = GetAttribute(f, 'd')
                # primary particle time
                primary_time[0] = GetAttribute(f, 'd')
                # primary particle position
                primary_position[0] = np.array([GetAttribute(f, 'd') for i in range(3)])
                # primary particle direction
                primary_direction[0] = np.array([GetAttribute(f, 'd') for i in range(3)])
                pri_counter = pri_counter + 1
            else:
                # primary particle name
                primary_name_length = GetAttribute(f, 'i')
                name_tmp = GetAttribute(f, 'c', primary_name_length)
                if save_primary_names:
                    primary_name = np.append(primary_name, name_tmp)
                # primary particle energy
                primary_energy = np.append(primary_energy, GetAttribute(f, 'd'))
                # primary particle time
                primary_time = np.append(primary_time, GetAttribute(f, 'd'))
                # primary particle position
                primary_position = np.r_[primary_position, np.reshape([GetAttribute(f, 'd') for i in range(3)], (1, 3))]
                # primary particle direction
                primary_direction = np.r_[primary_direction, np.reshape([GetAttribute(f, 'd') for i in range(3)], (1, 3))]
                pri_counter = pri_counter + 1

        #%% record size-independent entries
        # record level
        record_level[record_counter] = GetAttribute(f, 'i')
        # optical photon record level
        opticalphoton_record_level[record_counter] = GetAttribute(f, 'i')
        # thermalElectron record level
        thermalelectron_record_level[record_counter] = GetAttribute(f, 'i')
        # volume ID
        volume_id[record_counter] = GetAttribute(f, 'i')
        # Event #
        event_num[record_counter] = GetAttribute(f, 'i')
        # set the event number for each primary in this event
        primary_event_num[pri_counter_evt:pri_counter] = [event_num[record_counter] for i in range(pri_counter - pri_counter_evt)]
        # Energy deposition
        if record_level[record_counter] > 0:
            total_energy_deposition[record_counter] = GetAttribute(f, 'd')
        # Number of optical photons
        if opticalphoton_record_level[record_counter] > 0:
            num_opticalphotons[record_counter] = GetAttribute(f, 'i')
        # Number of thermal electrons
        if thermalelectron_record_level[record_counter] > 0:
            num_thermalelectrons[record_counter] = GetAttribute(f, 'i')
        # Record size
        hts_sz = GetAttribute(f, 'i')
        hits_size[record_counter] = int(hts_sz)

        record_hits_ind[record_counter] = step_counter
        if hits_size[record_counter] > 0:
            # read per-step variables in from file
            for ii_step in range(hits_size[record_counter]):
                if step_counter == 0:
                    # read particle name
                    particle_name_length = GetAttribute(f, 'i')
                    name = GetAttribute(f, 'c', particle_name_length)
                    if save_hit_names:
                        particle_name[0] = name
                    # read creator process name
                    creator_process_name_length = GetAttribute(f, 'i')
                    name = GetAttribute(f, 'c', creator_process_name_length)
                    if save_hit_names:
                        creator_process_name[0] = name
                    # read step process name
                    step_process_name_length = GetAttribute(f, 'i')
                    name = GetAttribute(f, 'c', step_process_name_length)
                    if save_hit_names:
                        step_process_name[0] = name
                    # read step number
                    step_num[0] = GetAttribute(f, 'i')
                    # read particle ID
                    particle_id[0] = GetAttribute(f, 'i')
                    # read track ID
                    track_id[0] = GetAttribute(f, 'i')
                    # read parent ID
                    parent_id[0] = GetAttribute(f, 'i')
                    # read particle energy
                    particle_energy[0] = GetAttribute(f, 'd')
                    # read particle direction
                    particle_direction[0] = [GetAttribute(f, 'd') for i in range(3)]
                    # read energy deposition
                    energy_deposition[0] = GetAttribute(f, 'd')
                    # read particle position
                    position[0] = [GetAttribute(f, 'd') for i in range(3)]
                    # read step time
                    step_time[0] = GetAttribute(f, 'd')
                    step_counter = step_counter + 1
                else:
                    # read particle name
                    particle_name_length = GetAttribute(f, 'i')
                    name = GetAttribute(f, 'c', particle_name_length)
                    if save_hit_names:
                        particle_name = np.append(particle_name, name)
                    # read creator process name
                    temp_name = name
                    creator_process_name_length = GetAttribute(f, 'i')
                    name = GetAttribute(f, 'c', creator_process_name_length)
                    if save_hit_names:
                        creator_process_name = np.append(creator_process_name, name)
                    # read step process name
                    temp_name = name
                    step_process_name_length = GetAttribute(f, 'i')
                    name = GetAttribute(f, 'c', step_process_name_length)
                    if save_hit_names:
                        step_process_name = np.append(step_process_name, name)
                    # read step number
                    step_num = np.append(step_num, GetAttribute(f, 'i'))
                    # read particle ID
                    particle_id = np.append(particle_id, GetAttribute(f, 'i'))
                    # read track ID
                    track_id = np.append(track_id, GetAttribute(f, 'i'))
                    # read parent ID
                    parent_id = np.append(parent_id, GetAttribute(f, 'i'))
                    # read particle energy
                    particle_energy = np.append(particle_energy, GetAttribute(f, 'd'))
                    # read particle direction
                    particle_direction = np.r_[particle_direction, np.reshape([GetAttribute(f, 'd') for i in range(3)], (1, 3))]
                    # read energy deposition
                    energy_deposition = np.append(energy_deposition, GetAttribute(f, 'd'))
                    # read particle position
                    position = np.r_[position, np.reshape([GetAttribute(f, 'd') for i in range(3)], (1, 3))]
                    # read step time
                    step_time = np.append(step_time, GetAttribute(f, 'd'))
                    step_counter = step_counter + 1

    if save_primary_names:
        primary_name = primary_name[0:pri_counter]
    primary_energy = primary_energy[0:pri_counter]
    primary_time = primary_time[0:pri_counter]
    primary_position = primary_position[0:pri_counter][:]
    primary_direction = primary_direction[0:pri_counter][:]
    primary_event_num = primary_event_num[0:pri_counter]

    if save_hit_names:
        particle_name = particle_name[0:step_counter]
        creator_process_name = creator_process_name[0:step_counter]
        step_process_name = step_process_name[0:step_counter]
    step_num = step_num[0:step_counter]
    particle_id = particle_id[0:step_counter]
    track_id = track_id[0:step_counter]
    parent_id = parent_id[0:step_counter]
    particle_energy = particle_energy[0:step_counter]
    particle_direction = particle_direction[0:step_counter][:]
    energy_deposition = energy_deposition[0:step_counter]
    position = position[0:step_counter][:]
    step_time = step_time[0:step_counter]

    # %%%%%%%%%%%%%%%%%%%%%%%% Assign variables %%%%%%%%%%%%%%%%%%%%%%%% %

    record['record_level'] = record_level
    record['opticalphoton_record_level'] = opticalphoton_record_level
    record['thermalelectron_record_level'] = thermalelectron_record_level
    record['volume_id'] = volume_id
    record['event_num'] = event_num
    record['total_energy_deposition'] = total_energy_deposition
    record['num_opticalphotons'] = num_opticalphotons
    record['num_thermalelectrons'] = num_thermalelectrons
    record['hits_size'] = hits_size
    record['record_hits_ind'] = record_hits_ind

    record['primary_name'] = primary_name
    record['primary_energy'] = primary_energy
    record['primary_time'] = primary_time
    record['primary_position'] = primary_position
    record['primary_direction'] = primary_direction
    record['primary_event_num'] = primary_event_num

    record['particle_name'] = particle_name
    record['creator_process_name'] = creator_process_name
    record['step_process_name'] = step_process_name
    record['step_num'] = step_num
    record['particle_id'] = particle_id
    record['track_id'] = track_id
    record['parent_id'] = parent_id
    record['particle_energy'] = particle_energy
    record['particle_direction'] = particle_direction
    record['energy_deposition'] = energy_deposition
    record['position'] = position
    record['step_time'] = step_time

    f.close()
    return record, info
