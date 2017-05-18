#-----------------------------------------------------------------------#
# The first line contains the dimension of matrix.			#
# The second one is a vector that contains all values of our entry.	#
# So, the program in C has to read the entry by line.			#
#-----------------------------------------------------------------------#

# enter with your path
path = "/Users/almeida/Dropbox/Unicamp/Disciplinas/MO644/Trabalho/input/matriz_"

k <- c(3, 5, 50, 100, 500, 1000) # dimensions
n <- 10000 # number that we'll use in matrix simulation

for(j in 1:length(k)){
	
	# guarantees that in every simulation the numbers generated are equals
	set.seed(1)
	
	M <- NULL
	M <- matrix(NA,n,k[j])
	
	# sampling
	for(i in 1:k[j]){
		M[,i] <- runif(n, 1, 50)
	}
	
	# create means for each column
	M_mean <- matrix(data=1, nrow=n) %*% apply(M, 2, mean)
	
	# creates a difference matrix
	D <- M - M_mean
	
	# creates the sample covariance matrix
	C <- cov(D)
	
	# avoid floating numbers
	C <- round(C)
	
	# TRUE: the matrix is positive definite
	# FALSE: the simulation failed
	print(is.positive.definite(C))
	
	# organizing the data with the dimension and the inputs of the matrix
	m_ps <- rep(NA, k[j]^2 + 1)
	m_ps[1] <- k[j]
	m_ps[2:length(m_ps)] <- as.vector(C)
	
	# saving the .in file
	write.table(rbind(m_ps), paste0(path, as.character(k[j]), "x", as.character(k[j]), ".in"), sep=" ", col.names = F, row.names = F)
	
}
