% Path to word length analysis tool
addpath /common/projects/acs/merle3/ptol0.7.2/ptolemy//src/domains/acs/utils/wordlength

% Path to the generated design and the name of the design
pathname='/common/projects/acs/merle3/ptol0.7.2/ptolemy//src/domains/acs/demo/magnitude';
dsnname='magnitude';

% Automatic execution script
cd(pathname);
wordgui

% List instructions for running the tool
fprintf('\n\nWordgui instructions can be found in\n');
fprintf('/common/projects/acs/merle3/ptol0.7.2/ptolemy//src/domains/acs/utils/wordlength/README.TXT\n');
