
#include "elements.h"


/*

int main()


{


	Vector4cd v1, v2, vt;
	v1 << 0.5, 0.5, 0.5, 0.5;
	v2 << -0.5, -0.5, -0.5, -0.5;
	Matrix4cd tmat;
	tmat <<
		0.707, 0, 0, 0.707,
		0.707, 0, 0, -0.707,
		0, 0.707, 0.707, 0,
		0, 0.707, -0.707, 0;
	return 0;
}
*/
	/*EPR ep1;
	ep1.print();
	QPair p1(&v1);
	QPair p2(&v2);
	p1.print();
	p2.print();
	p1 = &ep1.generate();
	p1.print();
	*/


/*


	vt << 0,0.707, 0.707,0;
	cout << endl << vt << endl << tmat << endl;
	cout << endl << tmat*vt << endl;
	vt << 0.707, 0, 0, 0.707;
	cout << endl << vt*vt.transpose() << endl;


	Vector4i vt1, vt2;
	vt1 << 0, 0, 0, 0;
	vt2 << 0, 0, 0, 0;
	Matrix <complex <double>, 16, 16> tmat23;
	for (int k = 0; k < 16; k++)
	{
		for (int j = 3; j > 0; j--)
		{
			if (vt1(j) > 1)
			{
				vt1(j - 1) = vt1(j - 1) + 1;
				vt1(j) = 0;
			}

		}
		vt2(0) = vt1(1);
		vt2(1) = vt1(2);
		vt2(2) = vt1(0);
		vt2(3) = vt1(3);
	
		cout << endl;
		for (int i=0;i<4;i++) cout << vt1(i) << " " << vt2(i) << endl;
		for (int i = 0; i < 16; i++)
		{
			tmat23(k, i) = (vt2(0) * 8 + vt2(1) * 4 + vt2(2) * 2 + vt2(3) == i);
		}

		vt1(3) = vt1(3) + 1;
	}
	cout << endl << tmat23 << endl;
	VectorXcd v16(16);
	v16 << 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15;
	cout << endl << v16 << endl << endl << tmat23*v16 << endl;
	//cout << v16
	//out << endl << Matrix <complex<double>, 16, 16>::Identity(16, 16) << endl;

	/*
	1-2-3-4 -> 2-3-1-4
	0000 -> 0000
	0001 -> 0001
	0010 -> 0100
	0011 -> 0101
	0100 -> 1000
	0101 -> 1001
	0110 -> 1100
	0111 -> 1101
	*/

/*
int main()
{


	Matrix<complex<double>, 16, 16> trmatmeas;
	Vector4cd bv[4];
	bv[0] << 1 / sqrt(2), 0, 0, 1 / sqrt(2);
	bv[1] << 1 / sqrt(2), 0, 0, -1 / sqrt(2);
	bv[2] << 0, 1 / sqrt(2), 1 / sqrt(2), 0;
	bv[3] << 0, 1 / sqrt(2), -1 / sqrt(2), 0;
	cout << bv[0] * bv[0].transpose() << endl;

	Matrix4cd auxmat;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			auxmat = bv[i] * bv[j].transpose();
			for (int k = 0; k < 16; k++)
			{
				trmatmeas(i * 4 + j, k) = auxmat(k);
			}
		}

	}

	cout << endl << trmatmeas << endl;
	cout << endl;
	Pair2Measure Bellm;
	Bellm.SetBellMeasure();
	//*Bellm.auxtransmat = Matrix <complex<double>, 16, 16>::Identity(16, 16);
	Bellm.print();
	QPair p1, p2;
	cout << "p1:" << endl; p1.print();
	cout << "p2:" << endl; p2.print();
	int res;
	res=Bellm.bmeasure(&p1, &p2);
	cout << endl << "result: " << res << endl << "p1:" << endl;
	p1.print();
	cout << endl << "p2: " << endl;
	p2.print();

	return 0;
}

*/
