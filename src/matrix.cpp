/***************************************************************************
 SocNetV: Social Network Visualizer 
 version: 2.2
 Written in Qt

                        matrix  -  description
                             -------------------
    copyright         : (C) 2005-2016 by Dimitris B. Kalamaras
    project site      : http://socnetv.org

 ***************************************************************************/

/*******************************************************************************
*     This program is free software: you can redistribute it and/or modify     *
*     it under the terms of the GNU General Public License as published by     *
*     the Free Software Foundation, either version 3 of the License, or        *
*     (at your option) any later version.                                      *
*                                                                              *
*     This program is distributed in the hope that it will be useful,          *
*     but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
*     GNU General Public License for more details.                             *
*                                                                              *
*     You should have received a copy of the GNU General Public License        *
*     along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
********************************************************************************/

#include "matrix.h"



#define TINY 1.0e-20

#include <cstdlib>		//allows the use of RAND_MAX macro
#include <QDebug>
#include <QtMath>		//needed for fabs, qFloor etc
#include <QTextStream>


/**
 * @brief Matrix::Matrix
 * Default constructor - creates a Matrix of given size (default 0)
 * Use resize(int) to resize it
 * @param Actors
 */
Matrix::Matrix (int rowDim, int colDim)  : m_rows (rowDim), m_cols(colDim) {
    row = new (nothrow) Row[ m_rows ];
    Q_CHECK_PTR( row );
    for (int i=0;i<m_rows; i++) {
        row[i].resize( m_cols );
    }
}



/**
* @brief Matrix::Matrix
* Copy constructor. Creates a Matrix identical to Matrix b
* Allows Matrix a=b declaration
* Every Row object holds max_int=32762
* @param b
*/
Matrix::Matrix(const Matrix &b) {
    qDebug()<< "Matrix:: constructor";
    m_rows=b.m_rows;
    m_cols=b.m_cols ;
    row = new Row[m_rows];
    Q_CHECK_PTR( row );
    for (int i=0;i<m_rows; i++) {
        row[i].resize( m_cols );
    }
    for (int i=0; i<m_rows; i++) {
        row[i]=b.row[i];
    }
}


/**
 * @brief Matrix::~Matrix
 * Destructor
 */
Matrix::~Matrix() {
    delete [] row;
}

 /**
 * @brief Matrix::clear
 * clears data
 */
void Matrix::clear() {
    if (m_rows > 0){
        qDebug() << "Matrix::clear() deleting old rows";
        m_rows=0;
        m_cols=0;
        delete [] row;
    }
}


/**
 * @brief Matrix::resize
 * Resize this matrix to m x n
 * Called before every operation on new matrices.
 * Every Row object holds max_int=32762
 * @param Actors
 */
void Matrix::resize (const int m, const int n) {
    qDebug() << "Matrix: resize() ";
    clear();
    m_rows = m;
    m_cols = n;
    row = new (nothrow) Row [ m_rows  ];
    Q_CHECK_PTR( row );
    qDebug() << "Matrix: resize() -- resizing each row";
    for (int i=0;i<m_rows; i++) {
        row[i].resize( m_cols );  // CHECK ME
    }
}




/**
 * @brief Matrix::findMinMaxValues
 * @param min value in the matrix
 * @param max value
 * Complexity: O(n^2)
 */
void Matrix::findMinMaxValues (float &min, float & max, bool &hasRealNumbers){
    max=0;
    min=RAND_MAX;
    hasRealNumbers = false;
    for (int r = 0; r < rows(); ++r) {
        for (int c = 0; c < cols(); ++c) {
            if ( fmod (item(r,c), 1.0)  != 0 )  {
                hasRealNumbers = true;
            }
            if ( item(r,c) > max) {
                max = item(r,c) ;
            }
            if ( item(r,c) < min){
                min = item(r,c) ;
            }
        }
    }
}



/**
 * @brief Matrix::NeighboursNearestFarthest
 *
 * Like Matrix::findMinMaxValues only it skips r==c
 *
 * @param min value. If (r,c) = minimum, it mean that neighbors r and c are the nearest in the matrix/network
 * @param max value
 * Complexity: O(n^2)
 */
void Matrix::NeighboursNearestFarthest (float &min, float & max,
                               int &imin, int &jmin,
                               int &imax, int &jmax){
    max=0;
    min=RAND_MAX;
    for (int r = 0; r < rows(); ++r) {
        for (int c = 0; c < cols(); ++c) {
            if (r==c) continue;
            if ( item(r,c) > max) {
                max = item(r,c) ;
                imax = r; jmax=c;
            }
            if ( item(r,c) < min){
                min = item(r,c) ;
                imin = r; jmin=c;
            }
        }
    }
}

/**
 * @brief Matrix::identityMatrix
 * makes this square matrix the identity square matrix I
 * @param dim
 */
void Matrix::identityMatrix(int dim) {
    qDebug() << "Matrix::identityMatrix() -- deleting old rows";
    clear();
    m_rows=dim;
    m_cols=dim;
    row = new (nothrow) Row [m_rows];
    Q_CHECK_PTR( row );
    //qDebug() << "Matrix: resize() -- resizing each row";
    for (int i=0;i<m_rows; i++) {
        row[i].resize(m_rows);
        setItem(i,i, 1);
    }
}



/**
 * @brief Matrix::zeroMatrix
 * makes this matrix the zero matrix of size mxn
 * @param m
 * @param n
 */
void Matrix::zeroMatrix(const int m, const int n) {
    qDebug() << "Matrix::zeroMatrix() m " << m << " n " << n;
    clear();
    m_rows=m;
    m_cols=n;
    row = new (nothrow) Row [m_rows];
    Q_CHECK_PTR( row );
    //qDebug() << "Matrix::zeroMatrix - resizing each row";
    for (int i=0;i<m_rows; i++) {
        row[i].resize(m_cols);
        setItem(i,i, 0);
    }

}


/**
 * @brief Matrix::item
 * returns the (r,c) matrix element
 * @param r
 * @param c
 * @return
 */
float Matrix::item( int r, int c ){
    return row[r].column(c);
}



/**
 * @brief Matrix::setItem
 * sets the (r,c) matrix element calling the setColumn method
 * @param r
 * @param c
 * @param elem
 */
void Matrix::setItem( const int r, const int c, const float elem ) {
    row [ r ].setColumn(c, elem);
}


/**
 * @brief Matrix::clearItem
 * clears the (r,c) matrix element
 * @param r
 * @param c
 */
void Matrix::clearItem( int r, int c ) 	{
    row[r].clearColumn(c);
}







/**
 * @brief Matrix::deleteRowColumn
 * @param erased
 */
void Matrix::deleteRowColumn(int erased){
    qDebug() << "Matrix:deleteRowColumn() - will delete row and column"
             << erased
             << "m_rows before" <<  m_rows;

    --m_rows;
    m_cols = m_rows;
    qDebug() << "Matrix:deleteRowColumn() - m_rows now " << m_rows << ". Resizing...";
    for (int i=0;i<m_rows+1; i++) {
        for (int j=0;j<m_rows+1; j++) {
            qDebug() << "Matrix:deleteRowColumn() -"
                        <<"item ("<< i+1 << "," << j+1 << ") ="<< item(i, j) ;
            if (i>=m_rows || j>=m_rows) {
                setItem( i, j, RAND_MAX) ;
                qDebug() << "Matrix:deleteRowColumn() -"
                         <<"both i,j>=m_rows, corner case (will be deleted). Settting to RAND_MAX."
                        << "New item value (" <<  i+1 << ", " << j+1 << ") ="<< item(i, j) ;
            }
            else if (i<erased && j< erased) {
                qDebug() << "Matrix:deleteRowColumn() -"
                         << "i, j < erased. Skipping. Item unchanged.";
                continue;
            }
            else if (i<erased && j>=erased) {
                setItem( i, j, item(i,j+1) ) ;
                qDebug() << "Matrix:deleteRowColumn() -"
                            <<"j>=erased, shifting column left"
                           << "New item value (" <<  i+1 << ", " << j+1 << ") ="<< item(i, j) ;
            }
            else if (i>=erased && j<erased) {
                setItem( i, j, item(i+1,j) ) ;
                qDebug() << "Matrix:deleteRowColumn() -"
                         <<"i>=erased, shifting rows up."
                        << "New item value (" <<  i+1 << ", " << j+1 << ") ="<< item(i, j) ;
            }
            else if (i>=erased && j>=erased) {
                setItem( i, j, item(i+1,j+1) ) ;
                qDebug() << "Matrix:deleteRowColumn() -"
                         <<"both i,j>=erased, shifting row up and column left."
                        << "New item value (" <<  i+1 << ", " << j+1 << ") ="<< item(i, j) ;
            }

        }
        row[i].setSize(m_cols);
    }
    qDebug() << "Matrix:deleteRowColumn() - finished, new matrix:";
    printMatrixConsole(true); // @TODO comment out to release

}


/**
 * @brief Matrix::fillMatrix
 * fills a matrix with a given value
 * @param value
 */
void Matrix::fillMatrix(float value )   {
    for (int i=0;i< rows() ; i++)
        for (int j=0;j< cols(); j++)
            setItem(i,j, value);
}



/**
 * @brief Matrix::subtractFromI
 * @return
 */
Matrix& Matrix::subtractFromI ()  {
    for (int i=0;i< rows();i++)
        for (int j=0;j<cols();j++) {
            if (i==j)
                setItem(i,j, 1.0 - item(i,j));
            else
                setItem(i,j, item(i,j));
        }
    return *this;
}




/**
 * @brief Matrix::swapRows
 * Swaps row A with row B of this matrix
 * @param rowA
 * @param rowB
 */
void Matrix::swapRows(int rowA,int rowB){
    qDebug()<<"   swapRow() "<< rowA+1 << " with " << rowB+1;
    float *tempRow = new  (nothrow) float [ rows() ];
    Q_CHECK_PTR(tempRow);
    for ( int j=0; j<  rows(); j++) {
      tempRow[j] = item (rowB, j);
      setItem ( rowB, j, item ( rowA, j ) );
      setItem ( rowA, j,  tempRow[j] );
      }
    delete [] tempRow;
}





/**
* @brief Matrix::multiplyScalar
  * Scalar Multiplication
  * Multiplies this by float f and returns the product matrix of the same dim
  * Allows to use P.multiplyScalar(f)
  * @param f
*/
void Matrix::multiplyScalar (const float  & f) {
        qDebug()<< "Matrix::multiplyScalar() with f " << f;
        for (int i=0;i< rows();i++) {
            for (int j=0;j<cols();j++) {
                setItem(i,j, item(i,j) * f );
            }
        }
}


/**
 * @brief Matrix::multiplyRow
 * Multiply every element of row A by value
 * @param row
 * @param value
 */
void Matrix::multiplyRow(int row, float value) {
    qDebug()<<"   multiplyRow() "<< row+1 << " by value " << value;
    for ( int j=0; j<  rows(); j++) {
        setItem ( row, j,  value * item (row, j) );
        qDebug()<<"   item("<< row+1 << ","<< j+1 << ") = " <<  item(row,j);
    }
}





/**
 * @brief Matrix::product
 * Matrix Multiplication
 * Allows P = product(a * b) where P, a and b are not the same initially.
 * Takes two matrices a and b of the same dimension
 * and returns their product as a reference to the calling object
 * NOTE: do not use it as B.product(A,B) because it will destroy B on the way.
 * @param a
 * @param b
 * @param symmetry
 * @return
 */
void Matrix::product( Matrix &a, Matrix & b, bool symmetry)  {
    qDebug()<< "Matrix::product()";
    for (int i=0;i< rows();i++)
        for (int j=0;j<cols();j++) {
            setItem(i,j,0);
            if (symmetry && i > j ) continue;
            for (int k=0;k<m_rows;k++) {
//                qDebug() << "Matrix::product() - a("<< i+1 << ","<< k+1 << ")="
//                         << a.item(i,k) << "* b("<< k+1 << ","<< j+1 << ")="
//                         << b.item(k,j)  << " gives "  << a.item(i,k)*b.item(k,j);
                setItem(i,j, item(i,j)+a.item(i,k)*b.item(k,j));
            }
            if (symmetry) {
                setItem(j,i, item(i,j) );
            }
            qDebug() << "Matrix::product() - ("<< i+1 << ","<< j+1 << ") = "
                     << item(i,j);
        }
}


/**
 * @brief Matrix::productSym
 * takes two ( N x N ) matrices (symmetric) and outputs an upper triangular matrix
 * @param a
 * @param b
 * @return
 */
Matrix& Matrix::productSym( Matrix &a, Matrix & b)  {
    for (int i=0;i<rows();i++)
        for (int j=0;j<cols();j++) {
            setItem(i,j,0);
            if (i>=j) continue;
            for (int k=0;k<m_rows;k++)
                if  ( k > j ) {
                    if (a.item(i,k)!=0 && b.item(j,k)!=0)
                        setItem(i,j, item(i,j)+a.item(i,k)*b.item(j,k));
                }
                else  //k <= j  && i<j
                    if ( i>k ) {
                        if (a.item(k,i)!=0 && b.item(k,j)!=0)
                            setItem(i,j, item(i,j)+a.item(k,i)*b.item(k,j));
                        }
                        else {
                            if (a.item(i,k)!=0 && b.item(k,j)!=0)
                                setItem(i,j, item(i,j)+a.item(i,k)*b.item(k,j));
                        }
        }
        return *this;
}


/**
 * @brief Matrix::pow
 * @param n
 * @param symmetry
 * @return
 * Returns the n power of this matrix
 */
Matrix& Matrix::pow (int n, bool symmetry)  {
    if (rows()!= cols()) {
        qDebug()<< "Matrix::pow() - Error. This works only for square matrix";
        return *this;
    }
    qDebug()<< "Matrix::pow() ";
    Matrix X, Y; //auxilliary matrices
    qDebug()<< "Matrix::pow() - creating X = this";
    X=*this; //X = this
    X.printMatrixConsole(true);
    qDebug()<< "Matrix::pow() - creating Y = I";
    Y.identityMatrix( rows() ); // y=I
    Y.printMatrixConsole(true);
    return expBySquaring2 (Y, X, n, symmetry);

}


/**
 * @brief Matrix::expBySquaring2
 * @param Y must be the Identity matrix  on first call
 * @param X the matrix to be powered
 * @param n the power
 * @param symmetry
 * @return Matrix&
 * Recursive algorithm implementing "Exponentiation by squaring".
 * On first call, parameters must be: Y=I, X the orginal matrix to power and n the power.
 * Returns the power of matrix X to this object.
 * Also known as Fast Modulo Multiplication, this algorithm allows
 * fast computation of a large power n of square matrix X
 * For n > 4 it is more efficient than naively multiplying the base with itself repeatedly.
 */
Matrix& Matrix::expBySquaring2 (Matrix &Y, Matrix &X,  int n, bool symmetry) {
    if (n==1) {
        qDebug() <<"Matrix::expBySquaring2() - n = 1. Computing PM = X*Y where "
                   "X = " ;
        X.printMatrixConsole();
        qDebug() <<"Matrix::expBySquaring2() - n = 1. And Y = ";
        Y.printMatrixConsole();
        Matrix *PM = new Matrix; PM->zeroMatrix(rows(), cols());
        PM->product(X, Y, symmetry);
        qDebug()<<"Matrix::expBySquaring2() - n = 1. PM = X*Y ="  ;
        PM->printMatrixConsole();
        return *PM;
    }
    else if ( n%2 == 0 ) { //even
        qDebug()<<"Matrix::expBySquaring2() - even n =" << n
               << "Computing PM = X * X";
        Matrix PM; PM.zeroMatrix(rows(), cols());
        PM.product(X,X,symmetry);
        qDebug()<<"Matrix::expBySquaring2() - even n =" << n << ". PM = X * X = " ;
        PM.printMatrixConsole();
        return expBySquaring2 ( Y, PM, n/2 );
    }
    else  { //odd
        qDebug()<<"Matrix::expBySquaring2() - odd n =" << n
               << "First compute PM = X * Y";
        Matrix PM, PM2;
        PM.zeroMatrix(rows(), cols());
        PM2.zeroMatrix(rows(), cols());
        PM.product(X,Y,symmetry);
        qDebug()<<"Matrix::expBySquaring2() - odd n =" << n << ". PM = X * Y = " ;
        PM.printMatrixConsole();
        qDebug()<<"Matrix::expBySquaring2() - odd n =" << n
               << "Now compute PM2 = X * X";
        PM2.product(X,X,symmetry);
        qDebug()<<"Matrix::expBySquaring2() - odd n =" << n << ". PM2 = X * X = " ;
        PM2.printMatrixConsole();
        return expBySquaring2 ( PM, PM2, (n-1)/2 );
    }
}

/**
 * @brief Matrix::sum
 * Matrix addition
 * Takes two (nxn) matrices and returns their sum as a reference to this
 * Same algorithm as operator +, just different interface.
 * In this case, you use something like: c.sum(a,b)
 * @param a
 * @param b
 * @return
 */
void Matrix::sum( Matrix &a, Matrix & b)  {
    for (int i=0;i< rows();i++)
        for (int j=0;j<cols();j++)
            setItem(i,j, a.item(i,j)+b.item(i,j));
}



/**
* @brief Matrix::operator =
* Assigment allows copying a matrix onto another using b=a where b,a matrices
* Equals two matrices.
* @param a
* @return
*/
Matrix& Matrix::operator = (Matrix & a) {
    qDebug()<< "Matrix::operator asignment =";
    if (this != &a){
        if (a.m_rows!=m_rows) {
            clear();
            m_rows=a.m_rows;
            m_cols=a.m_cols;
            row=new (nothrow) Row[m_rows];
			Q_CHECK_PTR( row );
            for (int i=0;i<m_rows; i++) {
                row[i].resize(m_cols); //every Row object holds max_int=32762
			}
		}
       for (int i=0;i<m_rows; i++)
           row[i]=a.row[i];
	}
	return *this;
}


/**
* @brief Matrix::operator +=
* Matrix add another matrix: +=
* Adds to this matrix another matrix B of the same dim and returns to this
* Allows A+=B
* @param b
* @return this
*/
void Matrix::operator +=(Matrix & b) {
    qDebug()<< "Matrix::operator +=";
    for (int i=0;i< rows();i++)
        for (int j=0;j<cols();j++)
            setItem(i,j, item(i,j)+b.item(i,j));


}


/**
  * @brief Matrix::operator +
  * Matrix addition: +
  * Adds this matrix and B of the same dim and returns the sum S
  * Allows S = A+B
  * @param b
  * @return Matrix S
*/
Matrix& Matrix::operator +(Matrix & b) {
    Matrix *S = new Matrix();
    S->zeroMatrix(rows(), cols());
    qDebug()<< "Matrix::operator +";
    for (int i=0;i< rows();i++)
        for (int j=0;j<cols();j++)
            S->setItem(i,j, item(i,j)+b.item(i,j));
    return *S;
}



/*
 * @brief Matrix::operator *
 * Matrix Multiplication
* Allows P = A * B where A,B of same dimension
* and returns product as a reference to the calling object
* NOTE: do not use it as B.product(A,B) because it will destroy B on the way.
* @param b
* @param symmetry
* @return
*/
Matrix& Matrix::operator *(Matrix & b) {

    qDebug()<< "Matrix::operator *";
    Matrix *P = new Matrix();
    P->zeroMatrix(rows(), cols());

    for (int i=0;i< rows();i++)
        for (int j=0;j<cols();j++) {
            P->setItem(i,j,0);
            for (int k=0;k<m_rows;k++) {
//                qDebug() << "Matrix::product() - a("<< i+1 << ","<< k+1 << ")="
//                         << a.item(i,k) << "* b("<< k+1 << ","<< j+1 << ")="
//                         << b.item(k,j)  << " gives "  << a.item(i,k)*b.item(k,j);
                    P->setItem(i,j, P->item(i,j) + item(i,k)*b.item(k,j) );

            }
            qDebug() << "Matrix::operator * - ("<< i+1 << ","<< j+1 << ") = "
                     << P->item(i,j);
        }
    return *P;
}




/**
 * @brief operator <<
 * Outputs matrix m to a text str
 * @param os
 * @param m
 * @return
 */
QTextStream& operator <<  (QTextStream& os, Matrix& m){
    qDebug() << "Matrix: << Matrix";
    int actorNumber=1, fieldWidth = 13;
    float maxVal, minVal, maxAbsVal, element;
    bool hasRealNumbers=false;

    m.findMinMaxValues(minVal, maxVal, hasRealNumbers);

    maxAbsVal = ( fabs(minVal) > fabs(maxVal) ) ? fabs(minVal) : fabs(maxVal) ;


    os << qSetFieldWidth(0) << endl ;

    os << "- Values:        "
       << ( (hasRealNumbers) ? ("real numbers (printed decimals 3)") : ("integers only" ) ) << endl;

    os << "- Max value:  ";

    if (maxVal == -1 ||  maxVal==RAND_MAX )
        os <<  infinity << " (=not connected nodes, in distance matrix)";
    else
        os <<   maxVal;

    os << qSetFieldWidth(0) << endl ;

    os << "- Min value:   ";

    if (minVal == -1 ||  minVal==RAND_MAX )
        os << infinity;
    else
        os << minVal;


    os << qSetFieldWidth(0) << endl << endl;

    os << qSetFieldWidth(7) << fixed << right << "v"<< qSetFieldWidth(3) << "" ;

    os <<  ( (hasRealNumbers) ? qSetRealNumberPrecision(3) : qSetRealNumberPrecision(0) ) ;

    // Note: In the case of Distance Matrix,
    // if there is DM(i,j)=RAND_MAX (not connected), we always use fieldWidth  = 13
    if ( maxAbsVal  > 999)
        fieldWidth  = 13 ;
    else if  ( maxAbsVal > 99)
        fieldWidth  = 10 ;
    else if ( maxAbsVal > 9   )
        fieldWidth  = 9 ;
    else
        fieldWidth  = 8 ;

    // print first/header row
    for (int r = 0; r < m.cols(); ++r) {
        actorNumber = r+1;

        if ( actorNumber > 999)
            os << qSetFieldWidth(fieldWidth-3) ;
        else if  ( actorNumber > 99)
            os << qSetFieldWidth(fieldWidth-2) ;
        else if ( actorNumber > 9)
            os << qSetFieldWidth(fieldWidth-1) ;
        else
            os << qSetFieldWidth(fieldWidth) ;

        os <<  fixed << actorNumber;
    }

    os << qSetFieldWidth(0) << endl;

    os << qSetFieldWidth(7)<< endl;

    // print rows
    for (int r = 0; r < m.rows(); ++r) {
        actorNumber = r+1;

        if ( actorNumber > 999)
            os << qSetFieldWidth(4) ;
        else if  ( actorNumber > 99)
            os << qSetFieldWidth(5) ;
        else if ( actorNumber > 9)
            os << qSetFieldWidth(6) ;
        else
            os << qSetFieldWidth(7) ;


        os <<  fixed << actorNumber
            << qSetFieldWidth(3) <<"" ;

        for (int c = 0; c < m.cols(); ++c) {
            element = m(r,c) ;
            os << qSetFieldWidth(fieldWidth) << fixed << right;
            if ( element == -1 || element == RAND_MAX)  // we print infinity symbol instead of -1 (distances matrix).
                os << fixed << right << qSetFieldWidth(fieldWidth) << INFINITY ; // do not use var "infinity" as it breaks formatting;
            else {
                if ( element > 999)
                    os << qSetFieldWidth(fieldWidth-3) ;
                else if  ( element > 99)
                    os << qSetFieldWidth(fieldWidth-2) ;
                else if ( element > 9)
                    os << qSetFieldWidth(fieldWidth-1) ;
                else
                    os << qSetFieldWidth(fieldWidth) ;
                os <<  element;
            }
        }
        os << qSetFieldWidth(0) << endl;
    }
    return os;
}




/**
 * @brief Matrix::printMatrixConsole
 * @return
 */
bool Matrix::printMatrixConsole(bool debug){
    //qDebug() << "Matrix::printMatrixConsole() debug " << debug ;
    QTextStream out ( (debug ? stderr : stdout) );

    for (int r = 0; r < rows(); ++r) {
        for (int c = 0; c < cols(); ++c) {

            if ( item(r,c) < RAND_MAX  ) {
                out <<  qSetFieldWidth(12) << qSetRealNumberPrecision(3)
                     <<  forcepoint << fixed<<right
                        << item(r,c);
            }
            else {
                out <<  qSetFieldWidth(12) << qSetRealNumberPrecision(3)
                     <<  forcepoint << fixed<<right
                        << "x";
            }

//            QTextStream( (debug ? stderr : stdout) )
//                    << ( (item(r,c) < RAND_MAX ) ? item(r,c) : INFINITY  )<<' ';
        }
        out <<qSetFieldWidth(0)<< endl;
    }
    return true;
}



Matrix& Matrix::pearsonCorrelationCoefficient(Matrix &A){
    qDebug()<< "Matrix::pearsonCorrelationCoefficient()";


    this->zeroMatrix(n,n);

    int N = 0;
    float sum = 0;
    float pcc = 0;

    bool rows = true;
    N = A.rows() ;

    QVector<float> mean (N,0); // holds mean values
    QVector<float> diffSums(N,0);
    QVector<float> squareSums(N,0);

    if (rows) {


        //compute mean values
        for (int i = 0 ; i < N ; i++ ) {
            sum = 0;
            for (int j = 0 ; j < N ; j++ ) {
                sum += AM.item(i,j);
            }
            mean[i] = sum / N;
            for (int j = 0 ; j < N ; j++ ) {
                diffSums [ i ]   +=  ( AM.item(i,j)  - mean[i] ) ;
                squareSums [ i ] +=  ( AM.item(i,j)  - mean[i] ) *  ( AM.item(i,j)  - mean[i] );
            }

        }

        for (int i = 0 ; i < N ; i++ ) {

            for (int j = 0 ; j < N ; j++ ) {
                diffSums [ i ]   +=  ( AM.item(i,j)  - mean[i] ) * ( AM.item(i,j)  - mean[j] )  ;
            }


            for (int j = 0 ; j < N ; j++ ) {
               pcc =   nom /  ( sqrt ( squareSums[i] ) * sqrt ( squareSums[j] ) );
            }

        }

    }





}



/**
 * @brief Matrix::inverseByGaussJordanElimination
 * Inverts given matrix A by Gauss Jordan elimination
   Input:  matrix A
   Output: matrix A becomes unit matrix
   *this becomes the invert of A and is returned back.
 * @param A
 * @return inverse matrix of A
 */
Matrix& Matrix::inverseByGaussJordanElimination(Matrix &A){
	qDebug()<< "Matrix::inverseByGaussJordanElimination()";
	int n=A.cols();
    qDebug()<<"Matrix::inverseByGaussJordanElimination() - build I size " << n
             << " This will become A^-1 in the end";

    identityMatrix( n );

	int l=0, m_pivotLine=0;
	float m_pivot=0, temp_pivot=0, elim_coef=0;

    for ( int j=0; j< n; j++) { // for n, it is the last diagonal element of A
	    l=j+1;
	    m_pivotLine=-1;
	    m_pivot = A.item(j,j);
	    qDebug() << "inverseByGaussJordanElimination() at column " << j+1
		    << " Initial pivot " << m_pivot ;
        for ( int i=l; i<n; i++) {
            temp_pivot = A.item(i,j);
            if ( qFabs( temp_pivot ) > qFabs ( m_pivot ) ) {
                qDebug() << " A("<< i+1 << ","<< j+1  << ") = " <<  temp_pivot
                         << " absolutely larger than current pivot "<< m_pivot
                         << ". Marking new pivot line: " << i+1;
                m_pivotLine=i;
                m_pivot = temp_pivot ;
            }
        }
        if ( m_pivotLine != -1 ) {
            A.swapRows(m_pivotLine,j);
            swapRows(m_pivotLine,j);
        }


	    qDebug()<<"   multiplyRow() "<< j+1 << " by value " << 1/m_pivot ;
        for ( int k=0; k<  rows(); k++) {
            A.setItem ( j, k,  (1/m_pivot) * A.item (j, k) );
            setItem ( j, k,  (1/m_pivot) * item (j, k) );
            qDebug()<<"   A.item("<< j+1 << ","<< k+1 << ") = " <<  A.item(j,k);
            qDebug()<<"   item("<< j+1 << ","<< k+1 << ") = " <<  item(j,k);
        }

	    qDebug() << "eliminate variables FromRowsBelow()" << j+1 ;
        for ( int i=0; i<  rows(); i++) {
		 qDebug()<<"   Eliminating item("<< i+1 << ","<< j+1 << ") = "
			 <<  A.item(i,j) << " while at column j="<<j+1;
		 if ( A.item(i,j)==0 ){
		    qDebug()<< " ...already eliminated - continue";
		    continue;
		}
		 if ( i == j){
		     qDebug()<< " ...skip pivotline - continue";
		    continue;
		}
		elim_coef=A.item (i, j);
        for ( int k=0; k<  cols(); k++) {
		    qDebug()<<"   A.item("<< i+1 << ","<< k+1 << ") = " <<  A.item(i,k)
			    << " will be subtracted by " << " A.item ("<< i+1
			    << ", "<< j+1 << ") x A.item(" << j+1 << ","<<k+1
			    <<") =" << elim_coef * A.item(j,k) ;
		    A.setItem ( i, k,   A.item (i, k) -  elim_coef * A.item(j, k)  );
		    qDebug()<<"   A.item("<< i+1 << ","<< k+1 << ") = " <<  A.item(i,k);

		    qDebug()<<"   item("<< i+1 << ","<< k+1 << ") = " <<  item(i,k)
			    << " will be subtracted by " << " A.item ("<< i+1
			    << ", "<< j+1 << ") x item(" << j+1 << ","<<k+1
			    <<") =" << elim_coef * item(j,k)  <<  " = "
			    << elim_coef << " x " << item(j,k) ;

		    setItem ( i, k,   item (i, k) -  elim_coef * item(j, k)  );
		    qDebug()<<"   item("<< i+1 << ","<< k+1 << ") = " <<  item(i,k);

		}
	    }

	}
	return *this;
}



/**
 * @brief Matrix::ludcmp(Matrix &a, const int &n, int *indx, float *d)
 * Given matrix a, it replaces a by the LU decomposition of a rowwise permutation of itself.
 * Used in combination with lubksb to solve linear equations or invert a matrix.
 * @param a: input matrix n x n and output arranged as in Knuth's equation (2.3.14)
 * @param n: input size of matrix
 * @param indx: output vector, records the row permutation effected by the partial pivoting
 * @param d: output as ±1 depending on whether the number of row interchanges was even or odd
 * @return:
 *
 * Code adapted from Knuth's Numerical Recipes in C, pp 46
 *
 */
bool Matrix::ludcmp (Matrix &a, const int &n, int indx[], float &d) {
    qDebug () << "Matrix::ludcmp () - decomposing matrix a to L*U";
    int i=0, j=0, imax=0, k;
    float big,temp;
    //vv=vector<float>(1,n);
    float *vv;            // vv stores the implicit scaling of each row
    vv=new (nothrow) float [n];
    Q_CHECK_PTR( vv );

//    QTextStream stream(stdout);
//    stream << "a = LU = " << a ;

    d=1.0;               // No row interchanges yet.

    qDebug () << "Matrix::ludcmp() - loop over row to get scaling info" ;
    for (i=0;i<n;i++) {  // Loop over rows to get the implicit scaling information.
        qDebug () << "Matrix::ludcmp() - row i " <<  i+1;
        big=0;
        for (j=0;j<n;j++) {
            if ((temp=fabs( a[i][j] ) ) > big)
                big=temp;
        }
        if (big == 0)  //       No nonzero largest element.
        {
            qDebug() << "Matrix::ludcmp() - Singular matrix in routine ludcmp";
            return false;
        }
        vv[i]=1.0/big;  //  Save the scaling.
        qDebug () << "Matrix::ludcmp() - big element in row i " << i+1 << " is "<< big << " row scaling vv[i] " << vv[i];
    }

    qDebug () << "Matrix::ludcmp() - Start Crout's LOOP over columns";

    for (j=0;j<n;j++) //     This is the loop over columns of Crout’s method.
    {
        qDebug () << "Matrix::ludcmp() - COLUMN j " <<  j+1 << " search largest pivot";
        big=0;  //      Initialize for the search for largest pivot element.
        imax = j;

        for (i=j;i<n;i++)
        {
            if ( ( temp = vv[i] * fabs( a[i][j] ) ) > big)
            {   //  Is the figure of merit for the pivot better than the best so far?
                big=temp;
                imax=i;
                qDebug () << "Matrix::ludcmp() - found new largest pivot at row " <<  imax+1 << " big " << temp;
            }
        }

        qDebug () << "Matrix::ludcmp() - check for row interchange ";
        if (j != imax) //          Do we need to interchange rows?
        {
            qDebug () << "Matrix::ludcmp() - interchanging rows " << imax+1 << " and " << j+1;
            for ( k=0; k<n; k++ ) { //            Yes, do so...
                temp=a[imax][k];
                a[imax][k] = a[j][k];
                a[j][k] = temp;
            }
            d = -(d);  //..and change the parity of d.
            vv[imax]=vv[j];  //         Also interchange the scale factor.
            qDebug () << "Matrix::ludcmp() - imax  " << imax+1  << " vv[imax]" << vv[imax] << "new parity d " << d;
        }
        indx[j]=imax;
        qDebug () << "Matrix::ludcmp() - indx[j]=imax=" <<  indx[j] +1;
        if ( a[j][j] == 0 ) {
            a[j][j] = TINY; // For some apps, on singular matrices, it is desirable to substitute TINY for zero.
            qDebug () << "Matrix::ludcmp() - WARNING singular matrix set a[j][j]=TINY ";
        }

        for (i=j+1;i<n;i++) {
            //     Now, divide by the pivot element.
            temp=a[i][j] /=  a[j][j] ;
            qDebug () << "Matrix::ludcmp() - j " << j+1<< " dividing by pivot " << a.item(j,j) << " temp  = " << temp;
            for (k=j+1;k<n;k++) {       //reduce remaining submatrix
                a[i][k] -= ( temp * a[j][k] );
                qDebug () << "Matrix::ludcmp() - lower a["<< i+1 << "][" << k+1 <<"] = " << a[i][k];
            }
        }


//           stream << endl << "at j " << j+1 << " matrix a = LU = " << a ;
    }  // Go back for the next column in the reduction.


 //free_vector(vv,1,n);
    qDebug () << "delete vector vv";
    delete[] vv;


//   stream << "final a = LU = " << a ;


    return true;

}




/**
 * @brief Matrix::lubksb(float **a, int n, int *indx, float b[])
 *
 * Solves the set of n linear equations A·X = b, where A nxn matrix
 * decomposed as L·U (L lower triangular and U upper triangular)
 * by forward substitution and  backsubstitution.
 *
 * Given A = L·U we have
 * A · x = (L · U) · x = L · (U · x) = b
 * So, this routine first solves
 * L · y = b
 * for the vector y by forward substitution and then solves
 * U · x = y
 * for the vector x using backsubstitution

 * @param a: input matrix a as the LU decomposition of A, returned by the routine ludcmp
 * @param n: input size of matrix
 * @param indx: input vector, records the row permutation, returned by the routine ludcmp
 * @param b: input array as the right-hand side vector B, and ouput with the solution vector X
 * @return:
 *
 * a, n, and indx are not modified by this routine and can be left in place for
 * successive calls with different right-hand sides b.
 * This routine takes into account the possibility that b will begin with many
 * zero elements, so it is efficient for use in matrix inversion.

* Code adapted from Knuth's Numerical Recipes in C, pp 47
 *
 */
void Matrix::lubksb(Matrix &a, const int &n, int indx[], float b[])
{
    qDebug () << "Matrix::lubksb() - ";
    int i, j, ii=0,ip;
    float sum;
    for ( i=0;i<n;i++) {  // When ii is set to a positive value, it will become the
        ip=indx[i];       // index of the first nonvanishing element of b. We now
        sum=b[ip];        // do the forward substitution, equation (2.3.6). The
        b[ip]=b[i];       // only new wrinkle is to unscramble the permutation
        if (ii != 0 )           // as we go.
            for ( j=(ii-1);j<=i-1;j++)
                sum -= a[i][j]*b[j];
        else if (sum !=0 )     // A nonzero element was encountered, so from now on we
            ii=i+1;         //  will have to do the sums in the loop above.
        qDebug() << "Matrix::lubksb() "<< "i " << i  << " ip=indx[i] " << ip <<  " b[ip] " << b[ip] << " b[i] " << b[i] <<  "sum " << sum ;
        b[i]=sum;
    }
    for ( i=(n-1);i>=0;i--) {  // Now we do the backsubstitution, equation (2.3.7).
        sum=b[i];
        qDebug() << "Matrix::lubksb() backsubstitution: "<< "i " << i  << " b[i] " << b[i] <<  "sum " << sum ;
        for ( j=i+1;j<n;j++)
            sum -= a[i][j]*b[j];
        b[i]=sum/a[i][i]; //  Store a component of the solution vector X. All done!
        qDebug() << "Matrix::lubksb() backsubstitution: "<< "i " << i  <<  "sum " << sum << " a[i][i] " << a[i][i]   << " b[i] " << b[i] ;
    }
}

/**
 * @brief Matrix::inverse
 * @param a
 * @return
 */
Matrix& Matrix::inverse(Matrix &a)
{
    int i,j, n=a.rows();
    float d, col[n];

    int indx[n];
    qDebug () << "Matrix::inverse() - inverting matrix a - size " << n;
    if (n==0) {
        return (*this);
    }
    if ( ! ludcmp(a,n,indx,d) )
    { //  Decompose the matrix just once.
        qDebug () << "Matrix::inverse() - matrix a singular - RETURN";
        return *this;
    }

    qDebug () << "Matrix::inverse() - find inverse by columns";
    for ( j=0; j<n; j++) {    //    Find inverse by columns.
        for( i=0; i<n; i++)
            col[i]=0;
        col[j]=1.0;

        qDebug () << "Matrix::inverse() - call lubksb";
        lubksb(a,n,indx,col);

        for( i=0; i<n; i++) {
             (*this)[i][j] = col[i];
        }

    }
        qDebug () << "Matrix::inverse() - finished!";

    return *this;
}

