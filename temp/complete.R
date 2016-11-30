complete <- function(directory, id = 1:332) {
    ids <- {}
    nobs <- {}
    for(i in id) {
        data <- read.csv(paste(directory, "/", formatC(i, width=3, flag="0"), ".csv", sep = ""))
        fullCaseCnt <- 0
        for(j in seq_len(nrow(data))) {
            if (sum(is.na(data[j,])) == 0) {
                fullCaseCnt <- fullCaseCnt + 1
            }
        }
        ids <- c(ids, i)
        nobs <- c(nobs, fullCaseCnt)
    }
    data.frame(id = ids, nobs = nobs)
}