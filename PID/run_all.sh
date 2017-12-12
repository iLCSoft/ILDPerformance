#####################################################
# Create all PID plots for a given LCIO file
# F.Gaede, DESY 2017
#
#####################################################

if [ $# -ne 2 ]; 
    then echo "usage:  . ./run_all.sh myfile.slcio Result_DIR"
    return 1
fi


LCIOFILE=$1
echo "will create PID plots for lcio file: ${LCIOFILE}"

RESDIR=$2
echo "will write plots to directory ${RESDIR}$"

# create the result directory if it does not yet exist
mkdir -p ${RESDIR}



# run Marlin to create the PID tree
##Marlin ./scripts/PIDTree.xml --global.LCIOInputFiles=${LCIOFILE} --MyAIDAProcessor.FileName=${RESDIR}/PID_tree



cd ./macros

macro="plotPIDs.C(\"../${RESDIR}/PID_tree.root\");"
root -b -q $macro

macro="plotPIDEfficiency.C(\"../${RESDIR}/PID_tree.root\");"
root -b -q $macro

macro="plotPIDPurity.C(\"../${RESDIR}/PID_tree.root\");"
root -b -q $macro

macro="plotdEdx.C(\"../${RESDIR}/PID_tree.root\");"
#root -b -q $macro

macro="plotPIDLike.C(\"../${RESDIR}/PID_tree.root\");"
#root -b -q $macro

cd -