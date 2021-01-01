% load file
files = dir(['../*.mat']);
struct = load(files(1).name);

figure(1);
clf();
% get array of all the energy deposits
% need to throw our phe hits, where no energy is depsited
cut_E_gt_zero = struct.record.energy_deposition > 0;
energy_deposits = struct.record.energy_deposition(cut_E_gt_zero);
hist(energy_deposits, 0:20);
xlabel('Recoil Energy (keV)');

figure(2);
clf();
% get R^2 and Z
% again, we need to avoid the entries where no energy is deposited
X = struct.record.position(cut_E_gt_zero, 1);
Y = struct.record.position(cut_E_gt_zero, 2);
Z = struct.record.position(cut_E_gt_zero, 3);

% turn X and Y into R^2
R2 = X.^2 + Y.^2;
plot(R2, Z, 'ko');
xlim([0, 625]);
ylim([0, 55]);
xlabel('R^2 (cm^2)');
ylabel('Z (cm)');

