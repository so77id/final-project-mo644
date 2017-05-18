
path = "/Users/almeida/Dropbox/Unicamp/Disciplinas/MO644/Trabalho/input/matriz_"

k <- c(3, 5, 50, 100, 500, 1000) # variables
n <- 10000 # observations

for(j in 1:length(k)){
	
	set.seed(1)
	
	M <- NULL
	M <- matrix(NA,n,k[j])
	
	for(i in 1:k[j]){
		M[,i] <- runif(n, 1, 50)
	}
	
	#create means for each column
	M_mean <- matrix(data=1, nrow=n) %*% apply(M, 2, mean)
	
	#creates a difference matrix
	D <- M - M_mean
	
	#creates the covariance matrix
	C <- cov(D) # sample covariance
	
	C <- round(C)
	
	print(is.positive.definite(C))
	
	m_ps <- rep(NA, k[j]^2 + 1)
	m_ps[1] <- k[j]
	m_ps[2:length(m_ps)] <- as.vector(C)
	
	write.table(rbind(m_ps), paste0(path, as.character(k[j]), "x", as.character(k[j]), ".in"), sep=" ", col.names = F, row.names = F)
	
}