% Advanced Computer Systems data 
% y axis is time in seconds to compress.
% x axis is the number of threads from 1 to 12

% small file buff = x
x1 = [1 2 3 4 5 6 7 8 9 10 11 12];
y1 = [1 2 3 4 5 6 7 8 9 10 11 12];

plot(x1,y1);
hold on

% medium file buff = x

x2 = [1 2 3 4 5 6 7 8 9 10 11 12];
y2 = [1 2 3 4 5 6 7 8 9 10 11 12];
plot(x2,y2);
title('Threads versus Compression Time');
legend('Small File n megs','Medium file n megs');
xlabel('Number of threads (max hardware threads = 12)');
ylabel('Time in seconds');