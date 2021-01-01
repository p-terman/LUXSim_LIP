function LUXSimMatlabReader(file_directory, files, save_directory, options)
% LUXSimMatlabReader(file_directory, files, save_directory, options)
%
% Reads LUXSim binary output files and saves the data in a Matlab struct
% array in file [BINARY_FILE_NAME].mat.
%
% Inputs:
%  file_directory - path to the directory containing the files to load. If
%   no argument is specified or if file_directory is an empty array, the
%   current directory is assumed.
%  files - cell array of file names to load. If there is no second argument
%   or if files is an empty array, all files are loaded. Argument can also 
%   be a non-cell-array partial file name for matching 
%   (i.e. >> files = 'LUXOut1';).
%  save_directory - path to the directory to save the Matlab files to. If
%   there is no third argument or if save_directory is an empty array,
%   files are saved to the binary file directory (file_directory).
%  options - structure with the following recognized fields:
%   .continuous - set to 1 for continuous mode -- LUXSimMatlabReader will
%    convert all existing files, then check for any new files and repeat.
%    (Note: will ignore last file in this mode to avoid opening incomplete
%    data files. This mode will also ignore the 'files' argument.) It is
%    recommended that you run LUXSimMatlabReader again with
%    options.overwrite (described below) set to 0 after simulation has
%    finished running, in order to process all data files. [Default: 0]
%   .delete_bin - set to 1 to delete .bin file after conversion. 
%    [Default: 0]
%	.gzip_bin - set to 1 to gzip .bin file after conversion. [Default: 0]
%   .overwrite - set to 1 to overwrite existing .mat files with the same
%    name as any .bin files found. [Default: 0]
%   .save_hit_names - set to 0 to not save particle and process names (these take up a lot
%	 of space). [Default: 1]
%   .save_primary_names - set to 0 to not save primary names (these take up a lot of 
%	 space). [Default: 1]
%   .use_mex - set to 0 to use Matlab functions only (i.e. if the MEX
%    function just isn't working). [Default: 1]
%
% Outputs:
%  Data is saved in struct array named 'record.' Please see the LUXSim
%  TWiki page for Matlab output format documentation.
%
% Changes:
%  v1.0 DCM 2009-09-29 - Initial submission.
%  v1.0 DCM 2009-10-01 - Update with LUXSimBinaryOutput.cc
%  v1.0 DCM 2009-10-22 - Update with LUXSimBinaryOutput.cc
%  v1.1 DCM 2010-02-23 - Added continuous mode and overwrite flags
%  v1.2 DCM 2010-03-01 - Fixed bug where record structure would retain some
%                        events from previously-loaded files
%  v2.0 DCM 2010-03-18 - Using MEX function to read in files for large
%                        speed increase. Moved informational variables into
%                        the 'info' variable. If MEX version fails to load
%                        file, reader will attempt to use non-MEX loading
%                        (used mainly for backwards compatibility)
%  v2.1 DCM 2010-04-14 - Added option for non-MEXed loading (workaround for
%                        memory leak)
%  v2.1 DCM 2010-05-05 - Update with LUXSimBinaryOutput.cc (now supports
%                        primary particle information)
%  v2.2 DCM 2010-05-17 - Bug fix in MEX code. Changed field name from
%                        "primaries" to "primary" for clarity
%  v2.3 DCM 2010-07-19 - Added delete_bin field to options structure
%  v2.4 DCM 2010-08-10 - Added filename matching functionality for second
%                        input argument
%  v2.5 KK  2011-12-02 - Added creator process name to the file read routines
%  DCM 2012-10-04 - Added gzip_bin option
%  DCM 2012-10-22 - Fix for gzip_bin so that original unzipped file gets
%                   deleted
%  DCM 2012-11-28 - Radical change to output format -- record is now a 1x1
%                   structure with data saved in arrays
%  DCM 2013-02-05 - Added the save_names option
%  DCM 2013-04-26 - Added event number for primary particles
%  DCM 2013-04-26 - Replaced save_names option with separate save_hit_names and
%					save_primary_names options
%  DCM 2013-05-21 - Added support for new output format (now includes
%                   primary time)
%  KK  2015-08-20 - Added step process name to the file read routines
% 


%% Input handling

if nargin < 1 || isempty(file_directory)
    file_directory = pwd;
elseif ~exist(file_directory,'dir') % check to make sure data directory
    % exists
    error('Directory %s does not exist\n',file_directory);
end
% check to make sure we have read permissions
[status,attribs] = fileattrib(file_directory);
if ~attribs.UserRead || ~attribs.UserExecute
    error('No permission to read directory %s\n',file_directory);
end

do_file_search = false;
if nargin < 2 || isempty(files)
    file_list = dir([file_directory filesep '*.bin']);
    
    if isempty(file_list)
        error('No files found in directory %s\n',file_directory);
    end
    
    files = {file_list.name};
    
    %files = cell(1,length(file_list));
    %for ii_file=1:length(file_list)
    %    files{ii_file} = file_list(ii_file).name;
    %end
elseif ~iscell(files)
    do_file_search = true;
    file_search = files;
    file_list = dir([file_directory filesep '*' files '*']);
    files = {file_list.name};
end

if nargin < 3 || isempty(save_directory)
    save_directory = file_directory;
elseif ~exist(save_directory,'dir') % check to make sure output directory
    % exists
    % attempt to create output directory
    [status,message] = mkdir(save_directory);
    
    % if creation is unsuccessful, quit
    if ~status
        error('Could not create output directory %s.\nError text:\n%s', ...
            save_directory, message);
    end
end

% check to make sure we have write permissions
[status,attribs] = fileattrib(save_directory);
if ~attribs.UserWrite || ~attribs.UserExecute
    error('No permission to write to directory %s\n',save_directory);
end

% process options structure
if nargin >= 4
    field_names = fieldnames(options);
    for ii=1:length(field_names)
        eval([field_names{ii} ' = options.' field_names{ii} ';']);
    end
end

if ~exist('continuous','var')
    continuous = false;
end

if ~exist('delete_bin','var')
    delete_bin = false;
end

if ~exist('gzip_bin','var')
	gzip_bin = false;
end

if ~exist('overwrite','var')
    overwrite = false;
end

if ~exist('save_hit_names','var')
    save_hit_names = true;
end

if ~exist('save_primary_names','var')
    save_primary_names = true;
end

%if ~exist('use_mex','var')
    use_mex = false
%end

if ~exist('LUXSimMatlabReader_LoadFile','file') && use_mex
    fprintf('MEXing LUXSimMatlabReader_LoadFile...\n');
    mex(which('LUXSimMatlabReader_LoadFile.c'));
    fprintf('Done!\n');
end


%% Load files

function_start_time = clock;

flag_run = true;
while flag_run
    
    % run endlessly if in continuous mode
    flag_run = continuous;
    
    % if we are in continuous mode, get file list, order by date created, and
    % ignore the last file
    if continuous
        
        fprintf('\nSearching for new files...\n');
        
        if do_file_search
            file_list = dir([file_directory filesep '*' file_search '*']);
        else
            file_list = dir([file_directory filesep '*.bin']);
        end
        
        file_dates = [file_list.datenum];
        file_names = {file_list.name};
        
        [dates_sort, ii_sort] = sort(file_dates);
        files = { file_names{ii_sort(1:end-1)} };
        
        overwrite = false;
        
    end
    
    for ii_file=1:length(files)
        
        file_start_time = clock;
        
        clear record info
        
        filename = files{ii_file};
        
        % check to see if we are overwriting, and if file already exists
        save_filename = strtok(filename,'.');
        if ~overwrite && exist([save_directory filesep save_filename '.mat'],'file')
            continue
        end
        
        fprintf('\nLoading file %s (%d/%d)\n',filename,ii_file,length(files));
        % Do all file loading / reading operations in LUXSimMatlabReader_LoadFile
        if use_mex
            [record,info] = LUXSimMatlabReader_LoadFile([file_directory, filesep, filename]);
        end
        
        if ~use_mex || isempty(record)
            [record,info] = LoadWithoutMEX([file_directory, filesep, filename],save_hit_names,save_primary_names);
            if isempty(record)
                fprintf('WARNING: no info found for file %s in directory %s\n', ...
                    filename, file_directory);
                disp('Moving on...');
                continue
            end
        end
        
        file_end_time = clock;
        fprintf('100%% loaded (%d evts / %d steps in %3.2f s)\n', ...
            length(record.total_energy_deposition), length(record.energy_deposition), ...
            etime(file_end_time, file_start_time));
        
        % figure out whether to save names
        if ~save_primary_names
            record = rmfield(record,'primary_name');
		end
		if ~save_hit_names
			record = rmfield(record,'particle_name');
            record = rmfield(record,'creator_process_name');
            record = rmfield(record,'step_process_name');
		end
        
        save_filename_fullpath = ...
            [save_directory filesep strtok(filename,'.') '.mat'];
        save(save_filename_fullpath, 'record', 'info', '-v7.3');
        
        fprintf('Data saved to %s\n', save_filename_fullpath);
        
        if gzip_bin
        	fprintf('Gzipping %s\n', [file_directory, filesep, filename]);
            gzip([file_directory, filesep, filename]);
            if exist([file_directory, filesep, filename, '.gz'],'file')
            	delete([file_directory, filesep, filename]);
            else
            	fprintf('WARNING: failed to gzip file %s\n', filename);
            end
        end
        
        if delete_bin
            fprintf('Deleting %s\n', [file_directory, filesep, filename]);
            delete([file_directory, filesep, filename]);
        end
    end
    
    function_end_time = clock;
    
    if continuous
        wait_time = 60;
        fprintf('\nChecking for new files at %s\n', ...
            datestr(now+wait_time/24/60/60));
        pause(wait_time);
    end
    
end

fprintf('\n************************************************************************************\n\n');
fprintf('All files processed (%1.0f s)\n\n', etime(function_end_time, function_start_time));

end


function [record,info] = LoadWithoutMEX(file,save_hit_names,save_primary_names)

% try
    
    tic;
    
    % open file
    fid = fopen(file);
    
    % error check
    if fid < 0
        % try tacking on a .bin suffix and reloading
        fid = fopen([file '.bin']);
        
        if fid < 0 % if still failing, give up
            fprintf('WARNING: unable to find file %s in directory %s\n',...
                filename,file_directory);
            disp('Moving on...');
            return
        else % correct filename and keep going
            filename = [filename '.bin'];
        end
    end
    
    % find binary file total size
    file_tmp = dir(file);
    filesize_bytes = file_tmp(1).bytes;
    
    if filesize_bytes == 0
        fprintf('WARNING: file %s is empty\n', file);
        disp('Moving on...');
        return
    end
    
    %%% Preamble
    
    % read number of records
    record_length = fread(fid,1,'int');
    
    % read production time
    production_time_length = fread(fid,1,'int');
    info.production_time = fread(fid,production_time_length,'*char')';
    
    % read Geant4 version
    geant4_version_length = fread(fid,1,'int');
    info.geant4_version = fread(fid,geant4_version_length,'*char')';
    
    % read revision number
    revision_number_length = fread(fid,1,'int');
    info.revision_number = fread(fid,revision_number_length,'*char')';
    
    % read computer name
    computer_name_length = fread(fid,1,'int');
    info.computer_name = fread(fid,computer_name_length,'*char')';
    
    %%% First-time run stuff
    if ~feof(fid)
        % read commands
        commands_length = fread(fid,1,'int');
        info.commands = fread(fid,commands_length,'*char')';
        
        % read differences
        svn_diff_length = fread(fid,1,'int');
        info.svn_diff = fread(fid,svn_diff_length,'*char')';
        
        % read detector components
        det_comp_length = fread(fid,1,'int');
        info.detector_components = fread(fid,det_comp_length,'*char')';
    end
    
    info.filename = file;
    
    
    %%% Particle records
    
    total_loaded_bytes = 28+...
        production_time_length+...
        geant4_version_length+...
        revision_number_length+...
        computer_name_length+...
        commands_length+...
        svn_diff_length+...
        det_comp_length;
    %record_counter = 1;
    pri_counter = 1;
    step_counter = 1;
    
    
    record_level = zeros(1,record_length);
    opticalphoton_record_level = zeros(1,record_length);
    thermalelectron_record_level = zeros(1,record_length);
    volume_id = zeros(1,record_length);
    event_num = zeros(1,record_length);
    total_energy_deposition = zeros(1,record_length);
    num_opticalphotons = zeros(1,record_length);
    num_thermalelectrons = zeros(1,record_length);
    hits_size = zeros(1,record_length);
    record_hits_ind = zeros(1,record_length);
    
    mult_p = 10;
    primary_name = {};
    if save_primary_names
		primary_name = cell(1,record_length*mult_p);
	end
    primary_energy = zeros(1,record_length*mult_p);
    primary_time = zeros(1,record_length*mult_p);
    primary_position = zeros(record_length*mult_p,3);
    primary_direction = zeros(record_length*mult_p,3);
    primary_event_num = zeros(1,record_length*mult_p);
    
    mult = 1000;
    particle_name = {};
    creator_process_name = {};
    step_process_name = {};
    if save_hit_names
		particle_name = cell(1,record_length*mult);
		creator_process_name = cell(1,record_length*mult);
        step_process_name = cell(1,record_length*mult);
	end
    step_num = zeros(1,record_length*mult);
    particle_id = zeros(1,record_length*mult);
    track_id = zeros(1,record_length*mult);
    parent_id = zeros(1,record_length*mult);
    particle_energy = zeros(1,record_length*mult);
    particle_direction = zeros(record_length*mult,3);
    energy_deposition = zeros(1,record_length*mult);
    position = zeros(record_length*mult,3);
    step_time = zeros(1,record_length*mult);
    
    % particle_name = {};
    % step_num = [];
    % particle_id = [];
    % track_id = [];
    % parent_id = [];
    % particle_energy = [];
    % particle_direction = [];
    % energy_deposition = [];
    % position = [];
    % step_time = [];
    
    update_time = 5; %s
    for record_counter=1:record_length
        time = toc;
        if time > update_time
            fprintf('%d%% loaded (%d evts / %d steps)\n',...
                floor(record_counter / record_length * 100), ...
                record_counter, step_counter);
            tic;
        end
        
        %%% primary particle info
        % primary particle size
        primary_size = fread(fid,1,'int');
        
        %primary_name = [primary_name cell(1,primary_size)];
        %primary_energy = [primary_energy zeros(1,primary_size)];
        %primary_position = [primary_position; zeros(primary_size,3)];
        %primary_direction = [primary_direction; zeros(primary_size,3)];
        
        pri_counter_save = pri_counter;
        pri_counter_evt = pri_counter;
        for ii_pri=1:primary_size
            % primary particle name
            primary_name_length = fread(fid,1,'int');
            name_tmp = fread(fid,primary_name_length,'*char')';
            if save_primary_names
				primary_name{pri_counter} = name_tmp;
			end
            % primary particle energy
            primary_energy(pri_counter) = fread(fid,1,'double');
            % primary particle time
            primary_time(pri_counter) = fread(fid,1,'double');
            % primary particle position
            primary_position(pri_counter,1:3) = ...
                fread(fid,3,'double');
            % primary particle direction
            primary_direction(pri_counter,1:3) = ...
                fread(fid,3,'double');
            
            pri_counter = pri_counter+1;
        end
        
        %%% record size-independent entries
        % record level
        record_level(record_counter) = fread(fid,1,'int');
        % optical photon record level
        opticalphoton_record_level(record_counter) = fread(fid,1,'int');
        % thermalElectron record level
        thermalelectron_record_level(record_counter) = fread(fid,1,'int');
        % volume ID
        volume_id(record_counter) = fread(fid,1,'int');
        % Event #
        event_num(record_counter) = fread(fid,1,'int');
        % set the event number for each primary in this event
        primary_event_num(pri_counter_evt:(pri_counter-1)) = ...
            event_num(record_counter);
        % Energy deposition
        if record_level(record_counter) > 0
            total_energy_deposition(record_counter) = fread(fid,1,'double');
        end
        % Number of optical photons
        if opticalphoton_record_level(record_counter) > 0
            num_opticalphotons(record_counter) = fread(fid,1,'int');
        end
        % Number of thermal electrons
        if thermalelectron_record_level(record_counter) > 0
            num_thermalelectrons(record_counter) = fread(fid,1,'int');
        end
        % Record size
        hts_sz = fread(fid,1,'int');
        hits_size(record_counter) = hts_sz;
        
        record_hits_ind(record_counter) = step_counter;
        
        if hits_size(record_counter) > 0
            % preallocate memory
            %particle_name = [particle_name cell(1,hts_sz)];
            %creator_process_name = [creator_process_name cell(1,hts_sz)];
            %step_process_name = [step_process_name cell(1,hts_sz)];
            %step_num = [step_num zeros(1,hts_sz)];
            %particle_id = [particle_id zeros(1,hts_sz)];
            %track_id = [track_id zeros(1,hts_sz)];
            %parent_id = [parent_id zeros(1,hts_sz)];
            %particle_energy = [particle_energy zeros(1,hts_sz)];
            %particle_direction = [particle_direction; zeros(hts_sz,3)];
            %energy_deposition = [energy_deposition zeros(1,hts_sz)];
            %position = [position; zeros(hts_sz,3)];
            %step_time = [step_time zeros(1,hts_sz)];
            
            % read per-step variables in from file
            for ii_step=1:hits_size(record_counter)
            	% read particle name
                particle_name_length = fread(fid,1,'int');
                name = fread(fid,particle_name_length,'*char')';
                if save_hit_names
					particle_name{step_counter} = name;
				end
				% read creator process name
                creator_process_name_length = fread(fid,1,'int');
                name = fread(fid,creator_process_name_length,'*char')';
                if save_hit_names
					creator_process_name{step_counter} = name;
                end
                % read step process name
                step_process_name_length = fread(fid,1,'int');
                name = fread(fid,step_process_name_length,'*char')';
                if save_hit_names
                	step_process_name{step_counter} = name;
                end
                % read step number
                step_num(step_counter) = fread(fid,1,'int');
                % read particle ID
                particle_id(step_counter) = fread(fid,1,'int');
                % read track ID
                track_id(step_counter) = fread(fid,1,'int');
                % read parent ID
                parent_id(step_counter) = fread(fid,1,'int');
                % read particle energy
                particle_energy(step_counter) = fread(fid,1,'double');
                % read particle direction
                particle_direction(step_counter,1:3) = fread(fid,3,'double');
                % read energy deposition
                energy_deposition(step_counter) = fread(fid,1,'double');
                % read particle position
                position(step_counter,1:3) = fread(fid,3,'double');
                % read step time
                step_time(step_counter) = fread(fid,1,'double');
                
                step_counter = step_counter+1;
            end
            
        end
    end
    
    fclose(fid);
    
    fprintf('100%% loaded (%d evts / %d steps)\n', ...
        record_counter, step_counter-1);
    
    if save_primary_names
		primary_name = primary_name(1:pri_counter-1);
	end
    primary_energy = primary_energy(1:pri_counter-1);
    primary_time = primary_time(1:pri_counter-1);
    primary_position = primary_position(1:pri_counter-1,:);
    primary_direction = primary_direction(1:pri_counter-1,:);
    primary_event_num = primary_event_num(1:pri_counter-1);
    
    if save_hit_names
		particle_name = particle_name(1:step_counter-1);
		creator_process_name = creator_process_name(1:step_counter-1);
        step_process_name = step_process_name(1:step_counter-1);
	end
    step_num = step_num(1:step_counter-1);
    particle_id = particle_id(1:step_counter-1);
    track_id = track_id(1:step_counter-1);
    parent_id = parent_id(1:step_counter-1);
    particle_energy = particle_energy(1:step_counter-1);
    particle_direction = particle_direction(1:step_counter-1,:);
    energy_deposition = energy_deposition(1:step_counter-1);
    position = position(1:step_counter-1,:);
    step_time = step_time(1:step_counter-1);
    
    % %%%%%%%%%%%%%%%%%%%%%%%% Assign variables %%%%%%%%%%%%%%%%%%%%%%%% %
    
    record.record_level = record_level;
    record.opticalphoton_record_level = opticalphoton_record_level;
    record.thermalelectron_record_level = thermalelectron_record_level;
    record.volume_id = volume_id;
    record.event_num = event_num;
    record.total_energy_deposition = total_energy_deposition;
    record.num_opticalphotons = num_opticalphotons;
    record.num_thermalelectrons = num_thermalelectrons;
    record.hits_size = hits_size;
    record.record_hits_ind = record_hits_ind;
    
    record.primary_name = primary_name;
    record.primary_energy = primary_energy;
    record.primary_time = primary_time;
    record.primary_position = primary_position;
    record.primary_direction = primary_direction;
    record.primary_event_num = primary_event_num;
    
    record.particle_name = particle_name;
    record.creator_process_name = creator_process_name;
    record.step_process_name = step_process_name;
    record.step_num = step_num;
    record.particle_id = particle_id;
    record.track_id = track_id;
    record.parent_id = parent_id;
    record.particle_energy = particle_energy;
    record.particle_direction = particle_direction;
    record.energy_deposition = energy_deposition;
    record.position = position;
    record.step_time = step_time;
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
% catch
    
%     fprintf('WARNING: error loading file %s\n', file);
%     record = [];
%     info = [];
    
% end

end
