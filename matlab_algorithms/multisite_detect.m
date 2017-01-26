% main script for running detection algorithms on test data
close all
addpath('test_data/sept29_2016_test/')
addpath('test_data/PhisioBank_iaf/')

% each .mat file contains a struct for data from a test
% currently, each struct has two fields:
% sampling_rate: the sampling rate of the data in Hz
% data: a NxC matrix where each column represents a channel and each row
%       represents a time point


%s = load('NormalSinusRhythm_struct.mat');
%s = load('Pacingfromchipapprox120bpmxmA_struct.mat');
%s = load('PacingfromMedtronic120bpm2mA_struct.mat');
s = load('iaf1_struct.mat'); %number ranges from 1-8 for different patients

Fs = s.Fs; %sampling rate
data = s.data;
numChannels = size(data,2);
begin_time = 0.0;
end_time = 25; %second
data = data(begin_time*Fs+1:end_time*Fs+1,:);

b = fir1(1000,2.5/Fs,'high'); %filter to remove DC bias
b2 = fir1(1000,150/Fs);

%%% run on each channel individually
ainds = zeros(numChannels, 1);
vinds = zeros(numChannels, 1);
for i=1:numChannels
    data(:,i) = filter(b,1,data(:,i));
    data(:,i) = filter(b2,1,data(:,i));
    [detection.v_length, detection.a_length] = LearnLengths(data(:,i));
%     detection.a_length = 30;
%     detection.v_length = 11;
    [detection.v_thresh,detection.a_thresh,detection.vflip,detection.aflip] = GuessParameters2(data(:,i),detection.v_length,detection.a_length);
    [vind, aind] = one_chamber_peak_finder(detection, data(:,i));
    
        
    figure; hold on;
    plot(data(:,i),'b');
    plot(vind, detection.v_thresh*detection.vflip, 'or');
    plot(aind, detection.a_thresh*detection.aflip, 'xk');
    title(['Channel' num2str(i)])
    
    ainds(i) = length(aind);
    vinds(i) = length(vind);
end
a=2;
% ainds
% vinds

