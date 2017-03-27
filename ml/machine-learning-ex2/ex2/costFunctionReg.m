function [J, grad] = costFunctionReg(theta, X, y, lambda)
%COSTFUNCTIONREG Compute cost and gradient for logistic regression with regularization
%   J = COSTFUNCTIONREG(theta, X, y, lambda) computes the cost of using
%   theta as the parameter for regularized logistic regression and the
%   gradient of the cost w.r.t. to the parameters.

% Initialize some useful values
m = length(y); % number of training examples

% You need to return the following variables correctly
J = 0;
grad = zeros(size(theta));

% ====================== YOUR CODE HERE ======================
% Instructions: Compute the cost of a particular choice of theta.
%               You should set J to the cost.
%               Compute the partial derivatives and set grad to the partial
%               derivatives of the cost w.r.t. each parameter in theta
n = size(theta);

for i = 1:m
    sig = sigmoid(X(i, :)*theta);
    J += -y(i)*log(sig) - (1 - y(i))*log(1 - sig);

    for j = 1:n
        grad(j) += (sig - y(i))*X(i, j);
    end
end

J /=m;

JTemp = 0;

for i = 2:n
    JTemp += theta(i)*theta(i);
end

JTemp = (JTemp*lambda) / (2*m);

J += JTemp;

% Calculate the grad now
grad = grad / m;
for i = 2:n
    grad(i) += (lambda*theta(i)) / m;
end


% =============================================================

end
