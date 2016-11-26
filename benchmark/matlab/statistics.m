clear; clc; close all; 
M = dlmread('../data/yelp_review.txt');
M_subs = M(: , 1:3);
M_vals = M(: , 4:4);
T = sptensor(M_subs, M_vals);


% Mode 
mode = 3;
[num, ~] = size(M_subs);

nnzs = zeros(1, T.size(mode));
for i = 1:num
    nnzs(M_subs(i, mode)) = nnzs(M_subs(i, mode)) + 1;
end

% plot
x = 1:length(nnzs);
plot(x, nnzs);
xlabel('Slice','FontSize',12);
ylabel('Non-zeros','FontSize',12);
title('Mode 2 non-zeros statistics','FontSize',12);
set(gca,'FontSize',12); 

saveas(gcf, '../../report/checkpoint/mode2.jpg')
