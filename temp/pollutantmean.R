pollutantmean <- function(direcory, pollutant, id = 1:332) {
    total <- {}
    for(i in id) {
        data <- read.csv(paste(direcory, "/", formatC(i, width=3, flag="0"), ".csv", sep = ""))
        x <- data[!is.na(data[pollutant]), pollutant]
        total <- c(total, x)
    }
    mean(total, na.rm = TRUE)
}