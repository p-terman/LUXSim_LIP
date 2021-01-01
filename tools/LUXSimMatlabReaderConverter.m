function [record_out,info_out] = LUXSimMatlabReaderConverter(file)
% [record,info] = LUXSimMatlabReaderConverter(file)
% 
% Converts newer LUXSimMatlabReader file output to the old reader format
% (arrays -> structure).
% 
% Inputs:
%  file - full path to the LUXSimMatlabReader output .mat file to be read
% 
% Outputs:
%  record - Nx1 structure containing standard LUXSimMatlabReader fields
%  info - 1x1 structure, copied unaltered from .mat file
% 
% For more information on outputs, see LUXSimMatlabReader documentation.
% 
% 2012-11-28 DCM - Original version
% 2013-03-07 DCM - Now supports LUXSimMatlabReader output built with
%                  save_names option set to 0
% 2013-05-21 DCM - Updated to support primary time
% 2015-08-20 KK  - Added the step process name to the processing
% 

if nargin == 0 || isempty(file)
    error('User must specify file to load');
end

if ~exist(file,'file')
    error('File %s does not exist',file);
end

load(file);

if ~exist('record','var') || ~exist('info','var')
    error('File %s does not have standard LUXSim output',file);
end

num_records = length(record.event_num);

fprintf('Loaded data saved from %s\n',info.filename);

if nargout > 1
    info_out = info;
end

record_out = struct('record_level', cell(1,num_records), ...
    'opticalphoton_record_level', cell(1,num_records), ...
    'thermalelectron_record_level', cell(1,num_records), ...
    'volume_id', cell(1,num_records), ...
    'event_num', cell(1,num_records), ...
    'total_energy_deposition', cell(1,num_records), ...
    'num_opticalphotons', cell(1,num_records), ...
    'num_thermalelectrons', cell(1,num_records), ...
    'record_size', cell(1,num_records), ...
    'primary', cell(1,num_records), ...
    'hits', cell(1,num_records));

for ii_r=1:num_records
    
    % add per-event records
    record_out(ii_r).record_level = record.record_level(ii_r);
    record_out(ii_r).opticalphoton_record_level = record.opticalphoton_record_level(ii_r);
    record_out(ii_r).thermalelectron_record_level = record.thermalelectron_record_level(ii_r);
    record_out(ii_r).volume_id = record.volume_id(ii_r);
    record_out(ii_r).event_num = record.event_num(ii_r);
    record_out(ii_r).total_energy_deposition = record.total_energy_deposition(ii_r);
    record_out(ii_r).num_opticalphotons = record.num_opticalphotons(ii_r);
    record_out(ii_r).num_thermalelectrons = record.num_thermalelectrons(ii_r);
    record_out(ii_r).record_size = record.hits_size(ii_r);
    
    % add primaries
    pri_ind = find(record.primary_event_num==record.event_num(ii_r));
    for ii_p=1:length(pri_ind)
        if isfield(record,'primary_name')
            record_out(ii_r).primary.name{ii_p} = record.primary_name{pri_ind(ii_p)};
        end
        record_out(ii_r).primary.energy(ii_p) = record.primary_energy(pri_ind(ii_p));
        record_out(ii_r).primary.time(ii_p) = record.primary_time(pri_ind(ii_p));
        record_out(ii_r).primary.position(ii_p,:) = record.primary_position(pri_ind(ii_p),:);
        record_out(ii_r).primary.direction(ii_p,:) = record.primary_direction(pri_ind(ii_p),:);
    end
    
    % add hits
    ind_start = record.record_hits_ind(ii_r);
    ind_end = record.record_hits_ind(ii_r) + record.hits_size(ii_r) - 1;
    
    hits_ind = ind_start:ind_end;
    
    if ~isempty(hits_ind)
        if isfield(record,'particle_name')
            record_out(ii_r).hits.particle_name = record.particle_name(hits_ind);
        end
        if isfield(record,'creator_process_name')
            record_out(ii_r).hits.creator_process_name = record.creator_process_name(hits_ind);
        end
        if isfield(record,'step_process_name')
            record_out(ii_r).hits.step_process_name = record.step_process_name(hits_ind);
        end
        record_out(ii_r).hits.step_num = record.step_num(hits_ind);
        record_out(ii_r).hits.particle_id = record.particle_id(hits_ind);
        record_out(ii_r).hits.track_id = record.track_id(hits_ind);
        record_out(ii_r).hits.parent_id = record.parent_id(hits_ind);
        record_out(ii_r).hits.particle_energy = record.particle_energy(hits_ind);
        record_out(ii_r).hits.particle_direction = record.particle_direction(hits_ind,:);
        record_out(ii_r).hits.energy_deposition = record.energy_deposition(hits_ind);
        record_out(ii_r).hits.position = record.position(hits_ind,:);
        record_out(ii_r).hits.step_time = record.step_time(hits_ind);
    end
    
end

end
