####**********************************************************************
####**********************************************************************
####
####  RANDOM FORESTS FOR SURVIVAL, REGRESSION, AND CLASSIFICATION (RF-SRC)
####  Version 2.2.0 (_PROJECT_BUILD_ID_)
####
####  Copyright 2016, University of Miami
####
####  This program is free software; you can redistribute it and/or
####  modify it under the terms of the GNU General Public License
####  as published by the Free Software Foundation; either version 3
####  of the License, or (at your option) any later version.
####
####  This program is distributed in the hope that it will be useful,
####  but WITHOUT ANY WARRANTY; without even the implied warranty of
####  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
####  GNU General Public License for more details.
####
####  You should have received a copy of the GNU General Public
####  License along with this program; if not, write to the Free
####  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
####  Boston, MA  02110-1301, USA.
####
####  ----------------------------------------------------------------
####  Project Partially Funded By: 
####  ----------------------------------------------------------------
####  Dr. Ishwaran's work was funded in part by DMS grant 1148991 from the
####  National Science Foundation and grant R01 CA163739 from the National
####  Cancer Institute.
####
####  Dr. Kogalur's work was funded in part by grant R01 CA163739 from the 
####  National Cancer Institute.
####  ----------------------------------------------------------------
####  Written by:
####  ----------------------------------------------------------------
####    Hemant Ishwaran, Ph.D.
####    Director of Statistical Methodology
####    Professor, Division of Biostatistics
####    Clinical Research Building, Room 1058
####    1120 NW 14th Street
####    University of Miami, Miami FL 33136
####
####    email:  hemant.ishwaran@gmail.com
####    URL:    http://web.ccs.miami.edu/~hishwaran
####    --------------------------------------------------------------
####    Udaya B. Kogalur, Ph.D.
####    Adjunct Staff
####    Department of Quantitative Health Sciences
####    Cleveland Clinic Foundation
####    
####    Kogalur & Company, Inc.
####    5425 Nestleway Drive, Suite L1
####    Clemmons, NC 27012
####
####    email:  ubk@kogalur.com
####    URL:    http://www.kogalur.com
####    --------------------------------------------------------------
####
####**********************************************************************
####**********************************************************************


var.select.rfsrc <-
  function(formula,          
           data,
           object,
           cause,
           outcome.target = NULL,
           method = c("md", "vh", "vh.vimp"),
           conservative = c("medium", "low", "high"),
           ntree = (if (method == "md") 1000 else 500),
           mvars = (if (method != "md") ceiling(ncol(data)/5) else NULL),
           mtry = (if (method == "md") ceiling(ncol(data)/3) else NULL),
           nodesize = 2,
           splitrule = NULL,
           nsplit = 10,
           xvar.wt = NULL,
           refit = (method != "md"),
           fast = FALSE,
           na.action = c("na.omit", "na.impute"),
           always.use = NULL,  
           nrep = 50,        
           K = 5,             
           nstep = 1,         
           prefit =  list(action = (method != "md"), ntree = 100, mtry = 500, nodesize = 3, nsplit = 1),
           do.trace = 0,       
           verbose = TRUE,
           ...
           )
{
  rfsrc.var.hunting <- function(train.id, var.pt, nstep) {
    if (verbose) cat("\t", paste("selecting variables using", mName), "...\n")
    drop.var.pt <- setdiff(var.columns, var.pt)
    if (grepl("surv", family)) {
      if (sum(data[train.id, 2], na.rm = TRUE) < 2) {
        stop("training data has insufficient deaths: K is probably set too high\n")
      }
    }
    rfsrc.filter.obj  <- rfsrc(rfsrc.all.f,
                               data=(if (LENGTH(var.pt, drop.var.pt)) data[train.id, -drop.var.pt]
                                       else data[train.id, ]),
                               ntree = ntree,
                               splitrule = splitrule,
                               nsplit = nsplit,
                               mtry = Mtry(var.columns, drop.var.pt),
                               nodesize = nodesize,
                               cause = cause,
                               na.action = na.action,
                               do.trace = do.trace,
                               importance="permute")
    if (rfsrc.filter.obj$family == "surv-CR") {
      target.dim <- max(1, min(cause, max(get.event.info(rfsrc.filter.obj)$event.type)), na.rm = TRUE)
    }
    imp <- get.imp(coerce.multivariate(rfsrc.filter.obj, outcome.target), target.dim)
    names(imp) <- rfsrc.filter.obj$xvar.names
    if (method == "vh.vimp") {
      VarStrength <- sort(imp, decreasing = TRUE)
      lower.VarStrength <- min(VarStrength) - 1 #need theoretical lower bound to vimp
      n.lower <- min(2, length(VarStrength))    #n.lower cannot be > no. available variables
      forest.depth <- m.depth <- NA
      sig.vars.old <- names(VarStrength)[1]
    }
      else {
        max.obj <- max.subtree(rfsrc.filter.obj, conservative = (conservative == "high"))
        if (is.null(max.obj$order)) {
          VarStrength <- lower.VarStrength <- 0
          forest.depth <- m.depth <- NA
          sig.vars.old <- names(sort(imp, decreasing = TRUE))[1]
        }
          else {
            m.depth <- VarStrength <- max.obj$order[, 1]
            forest.depth <- floor(mean(apply(max.obj$nodes.at.depth, 2, function(x){sum(!is.na(x))}), na.rm=TRUE))
            exact.threshold <- ifelse(conservative == "low", max.obj$threshold.1se, max.obj$threshold)
            n.lower <- max(min(2, length(VarStrength)),#n.lower cannot be > no. available variables
                           sum(VarStrength <= exact.threshold))
            VarStrength <- max(VarStrength) - VarStrength
            names(m.depth) <- names(VarStrength) <- rfsrc.filter.obj$xvar.names
            VarStrength <- sort(VarStrength, decreasing = TRUE)
            lower.VarStrength <- -1 #need theoretical upper bound to first order statistic
            sig.vars.old <- names(VarStrength)[1]
          }
      }
    nstep <- max(round(length(rfsrc.filter.obj$xvar.names)/nstep), 1)
    imp.old <- 0
    for (b in 1:nstep) {
      if (b == 1) {
        if (sum(VarStrength > lower.VarStrength) == 0) {
          sig.vars <- sig.vars.old
          break
        }
        n.upper <- max(which(VarStrength > lower.VarStrength), n.lower)
        threshold <- unique(round(seq(n.lower, n.upper, length = nstep)))
        if (length(threshold) < nstep) {
          threshold <- c(threshold, rep(max(threshold), nstep - length(threshold)))
        }
      }
      sig.vars <- names(VarStrength)[1:threshold[b]]
      if (!is.null(always.use)) {
        sig.vars <- unique(c(sig.vars, always.use))
      }
      if (length(sig.vars) <= 1) {#break if there is only one variable
        sig.vars <- sig.vars.old
        break
      }
      imp <- coerce.multivariate(vimp(rfsrc.filter.obj, sig.vars, outcome.target = outcome.target,
                                      joint = TRUE), outcome.target)$importance[target.dim]
      if (verbose) cat("\t iteration: ", b,
                       "  # vars:",     length(sig.vars),
                       "  joint-vimp:",  round(imp, 3),
                       "\r")
      if (imp  <= imp.old) {
        sig.vars <- sig.vars.old
        break
      }
        else {
          var.pt <- var.columns[match(sig.vars, xvar.names)]
          sig.vars.old <- sig.vars
          imp.old <- imp
        }
    }
    var.pt <- var.columns[match(sig.vars, xvar.names)]
    drop.var.pt <- setdiff(var.columns, var.pt)
    rfsrc.obj  <- rfsrc(rfsrc.all.f,
                        data=(if (LENGTH(var.pt, drop.var.pt)) data[train.id, -drop.var.pt]
                                else data[train.id, ]),
                        ntree = ntree,
                        splitrule = splitrule,
                        nsplit = nsplit,
                        cause = cause,
                        na.action = na.action,                  
                        do.trace = do.trace)
    return(list(rfsrc.obj=rfsrc.obj, sig.vars=rfsrc.obj$xvar.names, forest.depth=forest.depth, m.depth=m.depth))
  }
  if (!missing(object)) {
    if (sum(inherits(object, c("rfsrc", "grow"), TRUE) == c(1, 2)) != 2) {
      stop("This function only works for objects of class `(rfsrc, grow)'")
    }
    if (is.null(object$forest)) {
      stop("Forest is empty!  Re-run grow call with forest set to 'TRUE'")
    }
    rfsrc.all.f <- object$formula
  }
    else {
      if (missing(formula) || missing(data)) {
        if (sum(inherits(formula, c("rfsrc", "grow"), TRUE) == c(1, 2)) == 2) {
          object <- formula
        }
          else {
            stop("Need to specify 'formula' and 'data' or provide a grow forest object")
          }
      }
      rfsrc.all.f <- formula
    }
  if (!missing(object)) {
    outcome.target <- coerce.multivariate.target(object, outcome.target)
  }
  if (missing(object)) {
    formulaDetail <- finalizeFormula(parseFormula(rfsrc.all.f, data), data)
    family <- formulaDetail$family
    xvar.names <- formulaDetail$xvar.names
    yvar.names <- formulaDetail$yvar.names
    if (family != "unsupv") {
      data <- cbind(data[, yvar.names, drop = FALSE], data[, match(xvar.names, names(data))])
      yvar <- data[, yvar.names]
      yvar.dim <- ncol(cbind(yvar))
    }
      else {
        data <- data[, match(xvar.names, names(data))]
        yvar.dim <- 0
        method <- "md"
      }
  }
    else {
      family <- object$family
      xvar.names <- object$xvar.names
      if (family != "unsupv") {
        yvar.names <- object$yvar.names
        data <- data.frame(object$yvar, object$xvar)
        colnames(data) <- c(yvar.names, xvar.names)
        yvar <- data[, yvar.names]
        yvar.dim <- ncol(cbind(yvar))
      }
        else {
          data <- object$xvar
          yvar.dim <- 0
          method <- "md"
        }
    }
  if (missing(cause)) {
    cause <- 1
  }
  method <- match.arg(method, c("md", "vh", "vh.vimp"))
  conservative = match.arg(conservative, c("medium", "low", "high"))
  mName <- switch(method,
                  "md"      = "Minimal Depth",
                  "vh"      = "Variable Hunting",
                  "vh.vimp" = "Variable Hunting (VIMP)")
  rfsrc.all.f <- switch(family,
                        "surv"   = as.formula(paste("Surv(",yvar.names[1],",",yvar.names[2],") ~ .")),
                        "surv-CR"= as.formula(paste("Surv(",yvar.names[1],",",yvar.names[2],") ~ .")),
                        "regr"   = as.formula(paste(yvar.names, "~ .")),
                        "class"  = as.formula(paste(yvar.names, "~ .")),
                        "unsupv" = NULL,
                        "regr+"  = as.formula(paste("Multivar(", paste(yvar.names, collapse = ","), paste(") ~ ."), sep = "")),
                        "class+" = as.formula(paste("Multivar(", paste(yvar.names, collapse = ","), paste(") ~ ."), sep = "")),
                        "mix+"   = as.formula(paste("Multivar(", paste(yvar.names, collapse = ","), paste(") ~ ."), sep = ""))
                        )
  n <- nrow(data)
  P <- length(xvar.names)
  target.dim <- 1
  var.columns <- (1 + yvar.dim):ncol(data)
  if (!is.null(always.use)) {
    always.use.pt <- var.columns[match(always.use, xvar.names)]
  }
    else {
      always.use.pt <- NULL
    }
  xvar.wt <- get.grow.xvar.wt(xvar.wt, P)
  if (!is.null(mtry)) {
    mtry <- round(mtry)
    if (mtry < 1 | mtry > P) mtry <- max(1, min(mtry, P))
  }
  prefit.masterlist <- list(action = (method != "md"), ntree = 100, mtry = 500, nodesize = 3, nsplit = 1)
  parm.match <- na.omit(match(names(prefit), names(prefit.masterlist)))
  if (length(parm.match) > 0) {
    for (l in 1:length(parm.match)) {
      prefit.masterlist[[parm.match[l]]] <- prefit[[l]]
    }
  }
  prefit <- prefit.masterlist
  prefit.flag  <- prefit$action
  if (method == "md") {
    if (prefit.flag && is.null(xvar.wt) && missing(object)) {
      if (verbose) cat("Using forests to preweight each variable's chance of splitting a node...\n")
      rfsrc.prefit.obj  <- rfsrc(rfsrc.all.f,
                                 data = data,
                                 ntree = prefit$ntree,
                                 nodesize = prefit$nodesize,
                                 mtry = prefit$mtry,
                                 splitrule = splitrule,
                                 nsplit = prefit$nsplit,
                                 cause = cause,
                                 na.action = na.action,
                                 importance="permute")
      if (rfsrc.prefit.obj$family == "surv-CR") {
        target.dim <- max(1, min(cause, max(get.event.info(rfsrc.prefit.obj)$event.type)), na.rm = TRUE)
      }
      wts <- pmax(get.imp(coerce.multivariate(rfsrc.prefit.obj, outcome.target), target.dim), 0)
      if (any(wts > 0)) {
        xvar.wt <- get.grow.xvar.wt(wts, P)
      }
      rm(rfsrc.prefit.obj)
    }
    if (!missing(object)) {
    }
    if (!missing(object) && !prefit.flag) {
      if (verbose) cat("minimal depth variable selection ...\n")
      md.obj <- max.subtree(object, conservative = (conservative == "high"))
      object <- coerce.multivariate(object, outcome.target)
      outcome.target <- object$outcome.target
      pe <- get.err(object)
      ntree <- object$ntree
      nsplit <- object$nsplit
      mtry <- object$mtry
      nodesize <- object$nodesize
      if (family == "surv-CR") {
        target.dim <- max(1, min(cause, max(get.event.info(object)$event.type)), na.rm = TRUE)
      }
      imp <- get.imp(object, target.dim)
      imp.all <- get.imp.all(object)
      rm(object)
    }
      else {
        if (verbose) cat("running forests ...\n")
        rfsrc.obj <- rfsrc(rfsrc.all.f,
                           data,
                           ntree = ntree,
                           mtry = mtry,
                           nodesize = nodesize,
                           splitrule = splitrule,
                           nsplit = nsplit,
                           cause = cause,
                           na.action = na.action,
                           do.trace = do.trace,
                           xvar.wt = xvar.wt,
                           importance="permute")
        if (rfsrc.obj$family == "surv-CR") {
          target.dim <- max(1, min(cause, max(get.event.info(rfsrc.obj)$event.type)), na.rm = TRUE)
        }
        if (verbose) cat("minimal depth variable selection ...\n")
        md.obj <- max.subtree(rfsrc.obj, conservative = (conservative == "high"))
        rfsrc.obj <- coerce.multivariate(rfsrc.obj, outcome.target)
        outcome.target <- rfsrc.obj$outcome.target
        pe <- get.err(rfsrc.obj)
        imp <- get.imp(rfsrc.obj, target.dim)
        imp.all <- get.imp.all(rfsrc.obj)
        mtry <- rfsrc.obj$mtry
        nodesize <- rfsrc.obj$nodesize
        n <- nrow(rfsrc.obj$xvar)
        family <- rfsrc.obj$family
        rm(rfsrc.obj)#don't need the grow object
      }
    depth <- md.obj$order[, 1]
    threshold <- ifelse(conservative == "low", md.obj$threshold.1se, md.obj$threshold)
    top.var.pt <- (depth <= threshold)
    modelsize <- sum(top.var.pt)
    o.r.m <- order(depth, decreasing = FALSE)
    top.var.pt <- top.var.pt[o.r.m]
    varselect <- as.data.frame(cbind(depth = depth, vimp = imp.all))[o.r.m, ]
    topvars <- unique(c(always.use, rownames(varselect)[top.var.pt]))
    if (refit == TRUE) {
      if (verbose) cat("fitting forests to minimal depth selected variables ...\n")
      var.pt <- var.columns[match(topvars, xvar.names)]
      var.pt <- unique(c(var.pt, always.use.pt))
      drop.var.pt <- setdiff(var.columns, var.pt)
      rfsrc.refit.obj  <- rfsrc(rfsrc.all.f,
                                data=(if (LENGTH(var.pt, drop.var.pt)) data[, -drop.var.pt, drop = FALSE] else data),
                                ntree = ntree,
                                splitrule = splitrule,
                                nsplit = nsplit,
                                na.action = na.action,
                                do.trace = do.trace)
      rfsrc.refit.obj <- coerce.multivariate(rfsrc.refit.obj, outcome.target)
    }
      else {
        rfsrc.refit.obj <- NULL
      }
    if (verbose) {
      cat("\n\n")
      cat("-----------------------------------------------------------\n")
      cat("family             :", family, "\n")
      if (family == "regr+" | family == "class+" | family == "mix+") {
        cat("no. y-variables    : ", yvar.dim,       "\n", sep="")
        cat("response used      : ", outcome.target, "\n", sep="")
      }    
      cat("var. selection     :", mName, "\n")
      cat("conservativeness   :", conservative, "\n")
      cat("x-weighting used?  :", !is.null(xvar.wt), "\n")
      cat("dimension          :", P, "\n")
      cat("sample size        :", n, "\n")
      cat("ntree              :", ntree, "\n")
      cat("nsplit             :", nsplit, "\n")
      cat("mtry               :", mtry, "\n")
      cat("nodesize           :", nodesize, "\n")
      cat("refitted forest    :", refit, "\n")
      cat("model size         :", modelsize, "\n")
      cat("depth threshold    :", round(threshold, 4), "\n")
      if (!prefit.flag) {
        cat("PE (true OOB)      :", round(pe, 4), "\n")
      }
        else {
          cat("PE (biased)        :", round(pe, 4), "\n")
        }
      cat("\n\n")
      cat("Top variables:\n")
      print(round(varselect[top.var.pt, ], 3))
      cat("-----------------------------------------------------------\n")
    }
    return(invisible((list(err.rate=pe,
                           modelsize=modelsize,
                           topvars=topvars,
                           varselect=varselect,
                           rfsrc.refit.obj=rfsrc.refit.obj,
                           md.obj=md.obj
                           ))))
  }  
  pred.results <- dim.results <- forest.depth <- rep(0, nrep)
  var.signature <- NULL
  var.depth <- matrix(NA, nrep, P)
  outside.loop <- FALSE
  if (prefit.flag & is.null(xvar.wt)) {
    if (verbose) cat("Using forests to select a variables likelihood of splitting a node...\n")
    rfsrc.prefit.obj  <- rfsrc(rfsrc.all.f,
                               data = data,
                               ntree = prefit$ntree,
                               mtry = prefit$mtry,
                               nodesize = prefit$nodesize,
                               nsplit = prefit$nsplit,
                               cause = cause,
                               splitrule = splitrule,
                               na.action = na.action)
    if (rfsrc.prefit.obj$family == "surv-CR") {
      target.dim <- max(1, min(cause, max(get.event.info(rfsrc.prefit.obj)$event.type)), na.rm = TRUE)
    }
    outside.loop <- TRUE
  }
  for (m in 1:nrep) {
    if (verbose & nrep>1) cat("---------------------  Iteration:", m, "  ---------------------\n")
    all.folds <- switch(family,
                        "surv"     =  balanced.folds(yvar[, 2], K),
                        "surv-CR"  =  balanced.folds(yvar[, 2], K),
                        "class"    =  balanced.folds(yvar, K),
                        "regr"     =  cv.folds(n, K),
                        "class+"   =  balanced.folds(yvar, K),
                        "regr+"    =  cv.folds(n, K),
                        "mix+"     =  cv.folds(n, K)
                        )    
    if (fast == TRUE) {
      train.id <- all.folds[[1]]
      test.id <- all.folds[[2]]
    }
      else {
        test.id <- all.folds[[1]]
        train.id <- setdiff(1:n, test.id)
      }
    if (is.null(xvar.wt)) {
      if (!prefit.flag) {
        if (verbose) cat("Using forests to determine variable selection weights...\n")
        rfsrc.prefit.obj  <- rfsrc(rfsrc.all.f,
                                   data = data[train.id,, drop = FALSE],
                                   ntree = prefit$ntree,
                                   mtry = prefit$mtry,
                                   nodesize = prefit$nodesize,                                    
                                   nsplit = prefit$nsplit,
                                   cause = cause,
                                   splitrule = splitrule,
                                   na.action = na.action,
                                   importance = "permute")
        if (rfsrc.prefit.obj$family == "surv-CR") {
          target.dim <- max(1, min(cause, max(get.event.info(rfsrc.prefit.obj)$event.type)), na.rm = TRUE)
        }
      }
      rfsrc.prefit.obj <- coerce.multivariate(rfsrc.prefit.obj, outcome.target)
      wts <- pmax(get.imp(rfsrc.prefit.obj, target.dim), 0)
      if (any(wts > 0)) {
        var.pt <- unique(resample(var.columns, mvars, replace = TRUE, prob = wts))
      }
        else {
          var.pt <- var.columns[1:P]
        }
    }
      else {
        var.pt <- var.columns[1:P]
      }
    if (!is.null(xvar.wt)) {
      var.pt <- unique(resample(var.columns, mvars, replace = TRUE, prob = xvar.wt))
    }
    if (!is.null(always.use)) {
      var.pt <- unique(c(var.pt, always.use.pt))
    }
    object <- rfsrc.var.hunting(train.id, var.pt, nstep)
    rfsrc.obj <- object$rfsrc.obj
    outcome.target <- coerce.multivariate.target(rfsrc.obj, outcome.target)
    sig.vars <- object$sig.vars
    if (method == "vh") {
      forest.depth[m] <- object$forest.depth
      var.depth[m, match(names(object$m.depth), xvar.names)] <- object$m.depth
    }
    pred.out <- coerce.multivariate(predict(rfsrc.obj, data[test.id, ], importance = "none"), outcome.target)
    pred.results[m] <- get.err(pred.out)[target.dim] 
    dim.results[m] <- length(sig.vars)
    var.signature <- c(var.signature, sig.vars)
    if (verbose) {
      cat("\t                                                                \r")
      cat("\t PE:", round(pred.results[m], 4), "     dim:", dim.results[m], "\n")
    }
  }
  pred.results <- c(na.omit(pred.results))
  var.freq.all.temp <- 100 * tapply(var.signature, var.signature, length) / nrep
  freq.pt <- match(names(var.freq.all.temp), xvar.names)
  var.freq.all <- rep(0, P)
  var.freq.all[freq.pt] <- var.freq.all.temp
  if (method == "vh") {
    var.depth.all <- apply(var.depth, 2, mean, na.rm = T)
    varselect <- cbind(depth = var.depth.all, rel.freq = var.freq.all)
  }
    else {
      varselect <- cbind(rel.freq = var.freq.all)
    }
  o.r.f <- order(var.freq.all, decreasing = TRUE)
  rownames(varselect) <- xvar.names
  varselect <- varselect[o.r.f,, drop = FALSE]
  modelsize <- ceiling(mean(dim.results))  
  topvars <- unique(c(always.use, rownames(varselect)[1:modelsize]))
  if (refit == TRUE) {
    if (verbose) cat("fitting forests to final selected variables ...\n")
    var.pt <- var.columns[match(rownames(varselect)[1:modelsize], xvar.names)]
    drop.var.pt <- setdiff(var.columns, var.pt)
    rfsrc.refit.obj  <- rfsrc(rfsrc.all.f,
                              data = (if (LENGTH(var.pt, drop.var.pt)) data[, -drop.var.pt]
                                        else data),
                              na.action = na.action,
                              ntree = ntree,
                              nodesize = nodesize,
                              nsplit = nsplit,
                              cause = cause,
                              splitrule = splitrule,
                              do.trace = do.trace)
  }
    else {
      rfsrc.refit.obj <- NULL
    }
  if (verbose) {
    cat("\n\n")
    cat("-----------------------------------------------------------\n")
    cat("family             :", family, "\n")
    if (family == "regr+" | family == "class+" | family == "mix+") {
      cat("no. y-variables    : ", yvar.dim,              "\n", sep="")
      cat("response used      : ", outcome.target, "\n", sep="")
    }    
    cat("var. selection     :", mName, "\n")
    cat("conservativeness   :", conservative, "\n")
    cat("dimension          :", P, "\n")
    cat("sample size        :", n, "\n")
    cat("K-fold             :", K, "\n")
    cat("no. reps           :", nrep, "\n")
    cat("nstep              :", nstep, "\n")
    cat("ntree              :", ntree, "\n")
    cat("nsplit             :", nsplit, "\n")
    cat("mvars              :", mvars, "\n")
    cat("nodesize           :", nodesize, "\n")
    cat("refitted forest    :", refit, "\n")
    if (method == "vh") {
      cat("depth ratio        :", round(mean(mvars/(2^forest.depth)), 4), "\n")
    }
    cat("model size         :", round(mean(dim.results), 4), "+/-", round(SD(dim.results), 4), "\n")
    if (outside.loop) {
      cat("PE (K-fold, biased):", round(mean(pred.results), 4), "+/-", round(SD(pred.results), 4), "\n")
    }
      else {
        cat("PE (K-fold)        :", round(mean(pred.results), 4), "+/-", round(SD(pred.results), 4), "\n")
      }
    cat("\n\n")
    cat("Top variables:\n")
    print(round(varselect[1:modelsize,, drop = FALSE], 3))
    cat("-----------------------------------------------------------\n")
  }
  return(invisible(list(err.rate=pred.results,
                        modelsize=modelsize,
                        topvars=topvars,
                        varselect=varselect,
                        rfsrc.refit.obj=rfsrc.refit.obj,
                        md.obj=NULL
                        )))
}
get.imp <- function(f.o, target.dim) {
  if (!is.null(f.o$importance)) {
    c(cbind(f.o$importance)[, target.dim])
  }
    else {
      rep(NA, length(f.o$xvar.names))
    }
}
get.imp.all <- function(f.o) {
  if (!is.null(f.o$importance)) {
    imp.all <- cbind(f.o$importance)
    if (ncol(imp.all) == 1) {
      colnames(imp.all) <- "vimp"
    }
      else {
        colnames(imp.all) <- paste("vimp.", colnames(imp.all), sep = "")
      }
    imp.all
  }
    else {
      rep(NA, length(f.o$xvar.names))
    }
}
get.err <- function(f.o) {
  if (!is.null(f.o$err.rate)) {
    if (grepl("surv", f.o$family)) {
      err <- 100 * cbind(f.o$err.rate)[f.o$ntree, ]
    }
      else {
        err <- cbind(f.o$err.rate)[f.o$ntree, ]
      }
  }
    else {
      err = NA
    }
  err
}
SD <- function(x) {
  if (all(is.na(x))) {
    NA
  }
    else {
      sd(x, na.rm = TRUE)
    }
}
LENGTH <- function(x, y) {
  (length(x) > 0 & length(y) > 0)
}
Mtry <- function(x, y) {
  mtry <- round((length(x) - length(y))/3)
  if (mtry == 0) {
    round(length(x)/3)
  }
    else {
      mtry
    }
}
permute.rows <-function(x) {
  n <- nrow(x)
  p <- ncol(x)
  mm <- runif(length(x)) + rep(seq(n) * 10, rep(p, n))
  matrix(t(x)[order(mm)], n, p, byrow = TRUE)
}
balanced.folds <- function(y, nfolds = min(min(table(y)), 10)) {
  y[is.na(y)] <- resample(y[!is.na(y)], size = sum(is.na(y)), replace = TRUE)
  totals <- table(y)
  if (length(totals) < 2) {
    return(cv.folds(length(y), nfolds))
  }
    else {
      fmax <- max(totals)
      nfolds <- min(nfolds, fmax)     
      nfolds <- max(nfolds, 2)
      folds <- as.list(seq(nfolds))
      yids <- split(seq(y), y) 
      bigmat <- matrix(NA, ceiling(fmax/nfolds) * nfolds, length(totals))
      for(i in seq(totals)) {
        if(length(yids[[i]])>1){bigmat[seq(totals[i]), i] <- sample(yids[[i]])}
        if(length(yids[[i]])==1){bigmat[seq(totals[i]), i] <- yids[[i]]}
      }
      smallmat <- matrix(bigmat, nrow = nfolds)
      smallmat <- permute.rows(t(smallmat)) 
      res <- vector("list", nfolds)
      for(j in 1:nfolds) {
        jj <- !is.na(smallmat[, j])
        res[[j]] <- smallmat[jj, j]
      }
      return(res)
    }
}
var.select <- var.select.rfsrc
