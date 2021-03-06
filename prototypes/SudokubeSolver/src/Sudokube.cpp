/*
 * Sudokube.cpp
 *
 *  Created on: 2013-02-13
 *      Author: olivier
 */

#include "Sudokube.h"

const int CUBE_FACES = 3;
const int CUBE_FACE_HEIGHT = 4;
const int CUBE_FACE_LENGTH = 4;

Sudokube::Sudokube() {
	for(int i = 0; i < CUBE_FACES; i++) {
		for(int j = 0; j < CUBE_FACE_HEIGHT; j++) {
			for(int k = 0; k < CUBE_FACE_LENGTH; k++) {
				container[i][j][k] = new Case(i+1,j+1,k+1);
			}
		}
	}
}

Sudokube::~Sudokube() {
	Case* aCase;
	for(int i = 0; i < CUBE_FACES; i++) {
		for(int j = 0; j < CUBE_FACE_HEIGHT; j++) {
			for(int k = 0; k < CUBE_FACE_LENGTH; k++) {
				aCase = container[i][j][k];
				delete aCase;
			}
		}
	}
}

bool Sudokube::isSolved() {
	for(int i = 0; i < CUBE_FACES; i++) {
		for(int j = 0; j < CUBE_FACE_HEIGHT; j++) {
			for(int k = 0; k < CUBE_FACE_LENGTH; k++) {
				if(!(container[i][j][k]->isSolved())) {
					return false;
				}
			}
		}
	}
	return true;
}
int Sudokube::remainingCellsToBeSolved() {
	int remainingCells = 0;
	for(int i = 0; i < CUBE_FACES; i++) {
		for(int j = 0; j < CUBE_FACE_HEIGHT; j++) {
			for(int k = 0; k < CUBE_FACE_LENGTH; k++) {
				if(!(container[i][j][k]->isSolved())) {
					remainingCells++;
				}
			}
		}
	}
	return remainingCells;
}

void Sudokube::setCaseValue(int i, int j, int k, int value) {
	if(indexesOk(i,j,k)){
		container[i-1][j-1][k-1]->setValue(value);
	}
}

int Sudokube::getCaseValue(int i, int j, int k) {
	if(indexesOk(i,j,k)){
		return container[i-1][j-1][k-1]->getValue();
	}
	return 0;
}

void Sudokube::setRedCase(int i, int j, int k) {
	if(indexesOk(i,j,k)){
		redCase[0] = i;
		redCase[1] = j;
		redCase[2] = k;
	}
}

int* Sudokube::getRedCase() {
	return redCase;
}


void Sudokube::removePossibility(int i, int j, int k, int value) {
	if(indexesOk(i,j,k)){
		container[i-1][j-1][k-1]->removePossibility(value);
	}
}

vector<int> Sudokube::getPossibilities(int i, int j, int k) {
	vector<int> listOfPossibilities;
	if(indexesOk(i,j,k)){
		listOfPossibilities = container[i-1][j-1][k-1]->getPossibilities();
	}
	return listOfPossibilities;
}

void Sudokube::setPossibilities(int i, int j, int k, vector<int> possibilities) {
	if(indexesOk(i,j,k)){
		container[i-1][j-1][k-1]->setPossibilities(possibilities);
	}
}

bool Sudokube::indexesOk(int i, int j, int k) {
	if (	i >= 1 && i <= 3 &&
			j >= 1 && j <= 4 &&
			k >= 1 && k <= 4 ) {
		return true;
	}
	return false;
}

void Sudokube::print() {
	for(int k = 3; k >= 0; k--) {
		cout << "     ";
		for (int j = 0; j < 4; j++){
			cout <<container[2][j][k]->getValue() << " ";
		}
		cout << endl;
	}
	for(int x = 3; x >= 0; x--) {
		cout << container[0][x][3]->getValue() << " ";
		cout << container[0][x][2]->getValue() << " ";
		cout << container[0][x][1]->getValue() << " ";
		cout << container[0][x][0]->getValue() << "   ";
		cout << container[1][0][x]->getValue() << " ";
		cout << container[1][1][x]->getValue() << " ";
		cout << container[1][2][x]->getValue() << " ";
		cout << container[1][3][x]->getValue() << " ";
		cout << endl;
	}
}

bool Sudokube::removePossibilitiesFromConstraint(int i, int j, int k) {
	bool hasRemovedPossibilities = false;
	if(indexesOk(i,j,k) && container[i-1][j-1][k-1]->isSolved()) {
		int value = container[i-1][j-1][k-1]->getValue();

		vector<Case*> sameColumnCases = getSameColumnOfCase(i,j,k);
		for(unsigned int x = 0; x < sameColumnCases.size(); x++) {
			if(sameColumnCases[x]->contains(value)) {
				int nbPossibilities = sameColumnCases[x]->numberOfPossibilitiesRemaining();
				sameColumnCases[x]->removePossibility(value);
				if(nbPossibilities > sameColumnCases[x]->numberOfPossibilitiesRemaining()) {
					hasRemovedPossibilities = true;
				}
			}
		}

		vector<Case*> sameLineCases = getSameLineOfCase(i,j,k);
		for(unsigned int x = 0; x < sameLineCases.size(); x++) {
			if(sameLineCases[x]->contains(value)) {
				int nbPossibilities = sameLineCases[x]->numberOfPossibilitiesRemaining();
				sameLineCases[x]->removePossibility(value);
				if(nbPossibilities > sameLineCases[x]->numberOfPossibilitiesRemaining()) {
					hasRemovedPossibilities = true;
				}
			}
		}

		vector<Case*> sameRegionCases = getSameRegionOfCase(i,j,k);
		for(unsigned int x = 0; x < sameRegionCases.size(); x++) {
			if(sameRegionCases[x]->contains(value)) {
				int nbPossibilities = sameRegionCases[x]->numberOfPossibilitiesRemaining();
				sameRegionCases[x]->removePossibility(value);
				if(nbPossibilities > sameRegionCases[x]->numberOfPossibilitiesRemaining()) {
					hasRemovedPossibilities = true;
				}
			}
		}
	}
	return hasRemovedPossibilities;
}

bool Sudokube::removePossibilitiesFromNakedPairs(int i, int j, int k) {
	if(indexesOk(i,j,k)) {
		if(container[i-1][j-1][k-1]->numberOfPossibilitiesRemaining() == 2) {
			int possibility1 = container[i-1][j-1][k-1]->getPossibilities()[0];
			int possibility2 = container[i-1][j-1][k-1]->getPossibilities()[1];
			int nakedPair = 0;

			vector<Case*> sameColumnCases = getSameColumnOfCase(i,j,k);
			for(unsigned int x = 0; x < sameColumnCases.size(); x++) {
				if(		sameColumnCases[x]->numberOfPossibilitiesRemaining() == 2 &&
						sameColumnCases[x]->contains(possibility1) &&
						sameColumnCases[x]->contains(possibility2)) {
					nakedPair++;
				}
				if(nakedPair == 2) {
					for(unsigned int y = 0; y < sameColumnCases.size(); y++) {
						if(		sameColumnCases[y]->numberOfPossibilitiesRemaining() != 2 ||
								!sameColumnCases[y]->contains(possibility1) ||
								!sameColumnCases[y]->contains(possibility2)) {
							int possibilitiesRemaining = sameColumnCases[y]->numberOfPossibilitiesRemaining();
							sameColumnCases[y]->removePossibility(possibility1);
							sameColumnCases[y]->removePossibility(possibility2);
							if (possibilitiesRemaining > sameColumnCases[y]->numberOfPossibilitiesRemaining()) {
								return true;
							}
						}
					}
				}
			}

			nakedPair = 0;
			vector<Case*> sameLineCases = getSameLineOfCase(i,j,k);
			for(unsigned int x = 0; x < sameLineCases.size(); x++) {
				int nakedPair = 0;
				if(		sameLineCases[x]->numberOfPossibilitiesRemaining() == 2 &&
						sameLineCases[x]->contains(possibility1) &&
						sameLineCases[x]->contains(possibility2)) {
					nakedPair++;
				}
				if(nakedPair == 2) {
					for(unsigned int y = 0; y < sameLineCases.size(); y++) {
						if(		sameLineCases[y]->numberOfPossibilitiesRemaining() != 2 ||
								!sameLineCases[y]->contains(possibility1) ||
								!sameLineCases[y]->contains(possibility2)) {
							int possibilitiesRemaining = sameLineCases[y]->numberOfPossibilitiesRemaining();
							sameLineCases[y]->removePossibility(possibility1);
							sameLineCases[y]->removePossibility(possibility2);
							if (possibilitiesRemaining > sameLineCases[y]->numberOfPossibilitiesRemaining()) {
								return true;
							}
						}
					}
				}
			}

			nakedPair = 0;
			vector<Case*> sameRegionCases = getSameRegionOfCase(i,j,k);
			for(unsigned int x = 0; x < sameRegionCases.size(); x++) {
				int nakedPair = 0;
				if(		sameRegionCases[x]->numberOfPossibilitiesRemaining() == 2 &&
						sameRegionCases[x]->contains(possibility1) &&
						sameRegionCases[x]->contains(possibility2)) {
					nakedPair++;
				}
				if(nakedPair == 2) {
					for(unsigned int y = 0; y < sameRegionCases.size(); y++) {
						if(		sameRegionCases[y]->numberOfPossibilitiesRemaining() != 2 ||
								!sameRegionCases[y]->contains(possibility1) ||
								!sameRegionCases[y]->contains(possibility2)) {
							int possibilitiesRemaining = sameRegionCases[y]->numberOfPossibilitiesRemaining();
							sameRegionCases[y]->removePossibility(possibility1);
							sameRegionCases[y]->removePossibility(possibility2);
							if (possibilitiesRemaining > sameRegionCases[y]->numberOfPossibilitiesRemaining()) {
								return true;
							}
						}
					}
				}
			}

		}
	}
	return false;
}

bool Sudokube::removePossibilitiesFromHiddenPairs() {

	vector<vector<Case*> > allColumns;
	allColumns.push_back(getSameLineOfCase(1,1,1));
	allColumns.push_back(getSameLineOfCase(1,1,2));
	allColumns.push_back(getSameLineOfCase(1,1,3));
	allColumns.push_back(getSameLineOfCase(1,1,4));
	allColumns.push_back(getSameColumnOfCase(2,1,1));
	allColumns.push_back(getSameColumnOfCase(2,2,1));
	allColumns.push_back(getSameColumnOfCase(2,3,1));
	allColumns.push_back(getSameColumnOfCase(2,4,1));
	allColumns.push_back(getSameColumnOfCase(1,1,1));
	allColumns.push_back(getSameColumnOfCase(1,2,1));
	allColumns.push_back(getSameColumnOfCase(1,3,1));
	allColumns.push_back(getSameColumnOfCase(1,4,1));


	for(unsigned int a = 0; a < allColumns.size(); a++) {
		vector<Case*> column = allColumns[a];
		int value1;

		for(int m = 1; m <= 8; m++) {
			vector<int> index;
			for(unsigned int n = 0; n < column.size(); n++) {
				if(column[n]->contains(m)) {
					index.push_back(n);
				}
			}
			if(index.size() == 2) {
				value1 = m;
				Case* case1 = column[index[0]];
				Case* case2 = column[index[1]];
				vector<int> possibilities = case1->getPossibilities();
				bool isHiddenPair = false;

				for(unsigned int p = 0; p < possibilities.size(); p++) {
					if(possibilities[p] != value1 && case2->contains(possibilities[p])) {
						isHiddenPair = true;
						for(int q = 0; q < 8; q++) {
							if(q!= index[0] && q != index[1] && column[q]->contains(possibilities[p])) {
								isHiddenPair = false;
								break;
							}
						}
						if(isHiddenPair) {
							int case1PossibilitiesRemaining = case1->numberOfPossibilitiesRemaining();
							int case2PossibilitiesRemaining = case2->numberOfPossibilitiesRemaining();

							for(int r = 1; r <= 8; r++) {
								if(r!= value1 && r != possibilities[p]) {
									case1->removePossibility(r);
									case2->removePossibility(r);
								}
							}
							if (case1PossibilitiesRemaining > case1->numberOfPossibilitiesRemaining() ||
									case2PossibilitiesRemaining > case2->numberOfPossibilitiesRemaining()) {
								vector<int> test = case1->getPossibilities();
								return true;
							}
						}
					}
				}

			}
		}
	}
	return false;
}

bool Sudokube::removePossibilitiesFromHiddenTriples() {

	vector<vector<Case*> > allColumns;
	allColumns.push_back(getSameLineOfCase(1,1,1));
	allColumns.push_back(getSameLineOfCase(1,1,2));
	allColumns.push_back(getSameLineOfCase(1,1,3));
	allColumns.push_back(getSameLineOfCase(1,1,4));
	allColumns.push_back(getSameColumnOfCase(2,1,1));
	allColumns.push_back(getSameColumnOfCase(2,2,1));
	allColumns.push_back(getSameColumnOfCase(2,3,1));
	allColumns.push_back(getSameColumnOfCase(2,4,1));
	allColumns.push_back(getSameColumnOfCase(1,1,1));
	allColumns.push_back(getSameColumnOfCase(1,2,1));
	allColumns.push_back(getSameColumnOfCase(1,3,1));
	allColumns.push_back(getSameColumnOfCase(1,4,1));


	for(unsigned int a = 0; a < allColumns.size(); a++) {
		vector<Case*> column = allColumns[a];
		vector<vector<int> > hiddenTriplePossibilities;

		for(int m = 1; m <= 8; m++) {
			vector<int> index;
			for(unsigned int n = 0; n < column.size(); n++) {
				if(column[n]->contains(m)) {
					index.push_back(n);
				}
			}
			if(index.size() == 3) {
				vector<int> hiddenTriplePossibility;
				hiddenTriplePossibility.push_back(index[0]);
				hiddenTriplePossibility.push_back(index[1]);
				hiddenTriplePossibility.push_back(index[2]);
				hiddenTriplePossibility.push_back(m);
				hiddenTriplePossibilities.push_back(hiddenTriplePossibility);
			}
		}
		if(hiddenTriplePossibilities.size() >= 3) {
			int hiddenTripleCount;
			for(unsigned int p = 0; p < hiddenTriplePossibilities.size(); p++) {
				hiddenTripleCount = 1;
				vector<int> values;
				values.push_back(hiddenTriplePossibilities[p][3]);
				for(unsigned int q = 0; q < hiddenTriplePossibilities.size(); q++) {
					if(p != q &&
							hiddenTriplePossibilities[p][0] == hiddenTriplePossibilities[q][0] &&
							hiddenTriplePossibilities[p][1] == hiddenTriplePossibilities[q][1] &&
							hiddenTriplePossibilities[p][2] == hiddenTriplePossibilities[q][2]) {
						hiddenTripleCount++;
						values.push_back(hiddenTriplePossibilities[q][3]);
					}
				}
				if(hiddenTripleCount == 3) {
					int case1PossibilitiesCount = column[hiddenTriplePossibilities[p][0]]->numberOfPossibilitiesRemaining();
					int case2PossibilitiesCount = column[hiddenTriplePossibilities[p][1]]->numberOfPossibilitiesRemaining();
					int case3PossibilitiesCount = column[hiddenTriplePossibilities[p][2]]->numberOfPossibilitiesRemaining();

					for(int m = 1; m <= 8; m++) {
						if(m != values[0] && m != values[1] && m != values[2]) {
							column[hiddenTriplePossibilities[p][0]]->removePossibility(m);
							column[hiddenTriplePossibilities[p][1]]->removePossibility(m);
							column[hiddenTriplePossibilities[p][2]]->removePossibility(m);
						}
					}
					if(		case1PossibilitiesCount > column[hiddenTriplePossibilities[p][0]]->numberOfPossibilitiesRemaining() &&
							case2PossibilitiesCount > column[hiddenTriplePossibilities[p][1]]->numberOfPossibilitiesRemaining() &&
							case3PossibilitiesCount > column[hiddenTriplePossibilities[p][2]]->numberOfPossibilitiesRemaining()) {
						return true;
					}
				}

			}
		}
	}
	return false;
}

bool Sudokube::removePossibilitiesFromPointingPairs() {
	vector<vector<Case*> > allRegions;
	allRegions.push_back(getSameRegionOfCase(1,1,1));
	allRegions.push_back(getSameRegionOfCase(1,3,1));
	allRegions.push_back(getSameRegionOfCase(2,1,1));
	allRegions.push_back(getSameRegionOfCase(2,3,1));
	allRegions.push_back(getSameRegionOfCase(3,1,1));
	allRegions.push_back(getSameRegionOfCase(3,3,1));

	for(unsigned int a = 0; a < allRegions.size(); a++) {
		vector<Case*> region = allRegions[a];
		int value;

		for(int m = 1; m <= 8; m++) {
			vector<int> index;
			for(unsigned int n = 0; n < region.size(); n++) {
				if(region[n]->contains(m)) {
					index.push_back(n);
				}
			}
			if(index.size() == 2) {
				value = m;
				Case* case1 = region[index[0]];
				Case* case2 = region[index[1]];
				bool hasRemovedPossibility = false;


				if(case1->j == case2->j) {
					vector<Case*> sameColumn = getSameColumnOfCase(case1->i, case1->j, case1->k);
					for(unsigned int n = 0; n < sameColumn.size(); n++) {
						if(		!(sameColumn[n]->i == case1->i &&
								sameColumn[n]->j == case1->j &&
								sameColumn[n]->k == case1->k) &&
								!(sameColumn[n]->i == case2->i &&
								sameColumn[n]->j == case2->j &&
								sameColumn[n]->k == case2->k)) {
							if(sameColumn[n]->contains(value)) {
								sameColumn[n]->removePossibility(value);
								hasRemovedPossibility = true;
							}
						}
					}
				}else if(case1->k == case2->k) {
					vector<Case*> sameLine = getSameColumnOfCase(case1->i, case1->j, case1->k);
					for(unsigned int n = 0; n < sameLine.size(); n++) {
						if(		!(sameLine[n]->i == case1->i &&
								sameLine[n]->j == case1->j &&
								sameLine[n]->k == case1->k) &&
								!(sameLine[n]->i == case2->i &&
								sameLine[n]->j == case2->j &&
								sameLine[n]->k == case2->k)) {
							if(sameLine[n]->contains(value)) {
								sameLine[n]->removePossibility(value);
								hasRemovedPossibility = true;
							}
						}
					}
				}
				if (hasRemovedPossibility) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Sudokube::removePossibilitiesFromPointingTriples() {
	vector<vector<Case*> > allRegions;
	allRegions.push_back(getSameRegionOfCase(1,1,1));
	allRegions.push_back(getSameRegionOfCase(1,3,1));
	allRegions.push_back(getSameRegionOfCase(2,1,1));
	allRegions.push_back(getSameRegionOfCase(2,3,1));
	allRegions.push_back(getSameRegionOfCase(3,1,1));
	allRegions.push_back(getSameRegionOfCase(3,3,1));

	for(unsigned int a = 0; a < allRegions.size(); a++) {
		vector<Case*> region = allRegions[a];
		int value;

		for(int m = 1; m <= 8; m++) {
			vector<int> index;
			for(unsigned int n = 0; n < region.size(); n++) {
				if(region[n]->contains(m)) {
					index.push_back(n);
				}
			}
			if(index.size() == 3) {
				value = m;
				Case* case1 = region[index[0]];
				Case* case2 = region[index[1]];
				Case* case3 = region[index[2]];
				bool hasRemovedPossibility = false;


				if(case1->j == case2->j && case1->j == case3->j) {
					vector<Case*> sameColumn = getSameColumnOfCase(case1->i, case1->j, case1->k);
					for(unsigned int n = 0; n < sameColumn.size(); n++) {
						if(		!(sameColumn[n]->i == case1->i &&
								sameColumn[n]->j == case1->j &&
								sameColumn[n]->k == case1->k) &&
								!(sameColumn[n]->i == case2->i &&
								sameColumn[n]->j == case2->j &&
								sameColumn[n]->k == case2->k) &&
								!(sameColumn[n]->i == case3->i &&
								sameColumn[n]->j == case3->j &&
								sameColumn[n]->k == case3->k)) {
							if(sameColumn[n]->contains(value)) {
								sameColumn[n]->removePossibility(value);
								hasRemovedPossibility = true;
							}
						}
					}
				}else if(case1->k == case2->k && case1->k == case3->k) {
					vector<Case*> sameLine = getSameColumnOfCase(case1->i, case1->j, case1->k);
					for(unsigned int n = 0; n < sameLine.size(); n++) {
						if(		!(sameLine[n]->i == case1->i &&
								sameLine[n]->j == case1->j &&
								sameLine[n]->k == case1->k) &&
								!(sameLine[n]->i == case2->i &&
								sameLine[n]->j == case2->j &&
								sameLine[n]->k == case2->k) &&
								!(sameLine[n]->i == case3->i &&
								sameLine[n]->j == case3->j &&
								sameLine[n]->k == case3->k)) {
							if(sameLine[n]->contains(value)) {
								sameLine[n]->removePossibility(value);
								hasRemovedPossibility = true;
							}
						}
					}
				}
				if (hasRemovedPossibility) {
					return true;
				}
			}
		}
	}
	return false;
}


bool Sudokube::removePossibilitiesFromBoxLineReductionPair() {
	vector<vector<Case*> > allLines;
	allLines.push_back(getSameLineOfCase(1,1,4));
	allLines.push_back(getSameLineOfCase(1,1,3));
	allLines.push_back(getSameLineOfCase(1,1,2));
	allLines.push_back(getSameLineOfCase(1,1,1));
	allLines.push_back(getSameColumnOfCase(1,1,1));
	allLines.push_back(getSameColumnOfCase(1,2,1));
	allLines.push_back(getSameColumnOfCase(1,3,1));
	allLines.push_back(getSameColumnOfCase(1,4,1));
	allLines.push_back(getSameColumnOfCase(2,1,1));
	allLines.push_back(getSameColumnOfCase(2,2,1));
	allLines.push_back(getSameColumnOfCase(2,3,1));
	allLines.push_back(getSameColumnOfCase(2,4,1));

	for(unsigned int a = 0; a < allLines.size(); a++) {
		vector<Case*> lines = allLines[a];
		int value;

		for(int m = 1; m <= 8; m++) {
			vector<int> index;
			for(unsigned int n = 0; n < lines.size(); n++) {
				if(lines[n]->contains(m)) {
					index.push_back(n);
				}
			}
			if(index.size() == 2) {
				value = m;
				Case* case1 = lines[index[0]];
				Case* case2 = lines[index[1]];
				bool hasRemovedPossibility = false;


				if((case1->i == case2->i) && ((case1->j <= 2 && case2->j <= 2) || (case1->j > 2 && case2->j > 2))) {
					vector<Case*> sameRegion = getSameRegionOfCase(case1->i, case1->j, case1->k);
					for(unsigned int n = 0; n < sameRegion.size(); n++) {
						if(		!(sameRegion[n]->i == case1->i &&
								sameRegion[n]->j == case1->j &&
								sameRegion[n]->k == case1->k) &&
								!(sameRegion[n]->i == case2->i &&
								sameRegion[n]->j == case2->j &&
								sameRegion[n]->k == case2->k)) {
							if(sameRegion[n]->contains(value)) {
								sameRegion[n]->removePossibility(value);
								hasRemovedPossibility = true;
							}
						}
					}
				}
				if (hasRemovedPossibility) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Sudokube::removePossibilitiesFromBoxLineReductionTriple() {
	vector<vector<Case*> > allLines;
	allLines.push_back(getSameLineOfCase(1,1,4));
	allLines.push_back(getSameLineOfCase(1,1,3));
	allLines.push_back(getSameLineOfCase(1,1,2));
	allLines.push_back(getSameLineOfCase(1,1,1));
	allLines.push_back(getSameColumnOfCase(1,1,1));
	allLines.push_back(getSameColumnOfCase(1,2,1));
	allLines.push_back(getSameColumnOfCase(1,3,1));
	allLines.push_back(getSameColumnOfCase(1,4,1));
	allLines.push_back(getSameColumnOfCase(2,1,1));
	allLines.push_back(getSameColumnOfCase(2,2,1));
	allLines.push_back(getSameColumnOfCase(2,3,1));
	allLines.push_back(getSameColumnOfCase(2,4,1));

	for(unsigned int a = 0; a < allLines.size(); a++) {
		vector<Case*> lines = allLines[a];
		int value;

		for(int m = 1; m <= 8; m++) {
			vector<int> index;
			for(unsigned int n = 0; n < lines.size(); n++) {
				if(lines[n]->contains(m)) {
					index.push_back(n);
				}
			}
			if(index.size() == 3) {
				value = m;
				Case* case1 = lines[index[0]];
				Case* case2 = lines[index[1]];
				Case* case3 = lines[index[2]];
				bool hasRemovedPossibility = false;


				if((case1->i == case2->i && case1->i == case3->i) && ((case1->j <= 2 && case2->j <= 2 && case3->j <=2) ||
						(case1->j > 2 && case2->j > 2 && case3->j >2))) {
					vector<Case*> sameRegion = getSameRegionOfCase(case1->i, case1->j, case1->k);
					for(unsigned int n = 0; n < sameRegion.size(); n++) {
						if(		!(sameRegion[n]->i == case1->i &&
								sameRegion[n]->j == case1->j &&
								sameRegion[n]->k == case1->k) &&
								!(sameRegion[n]->i == case2->i &&
								sameRegion[n]->j == case2->j &&
								sameRegion[n]->k == case2->k) &&
								!(sameRegion[n]->i == case3->i &&
								sameRegion[n]->j == case3->j &&
								sameRegion[n]->k == case3->k)) {
							if(sameRegion[n]->contains(value)) {
								sameRegion[n]->removePossibility(value);
								hasRemovedPossibility = true;
							}
						}
					}
				}
				if (hasRemovedPossibility) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Sudokube::removePossibilitiesFromXWing() {
//	vector<vector<Case*> > allColumns;
//	allColumns.push_back(getSameLineOfCase(1,1,1));
//	allColumns.push_back(getSameLineOfCase(1,1,2));
//	allColumns.push_back(getSameLineOfCase(1,1,3));
//	allColumns.push_back(getSameLineOfCase(1,1,4));
//
//	vector<vector<int> >xWingPossibilities;
//
//	for(unsigned int a = 0; a < allColumns.size(); a++) {
//		vector<Case*> column = allColumns[a];
//
//		for(int m = 1; m <= 8; m++) {
//			vector<int> index;
//			for(unsigned int n = 0; n < column.size(); n++) {
//				if(column[n]->contains(m)) {
//					index.push_back(n);
//				}
//			}
//			if(index.size() == 2) {
//				vector<int> xWingPossibility;
//				xWingPossibility.push_back(a);
//				xWingPossibility.push_back(index[0]);
//				xWingPossibility.push_back(index[1]);
//				xWingPossibility.push_back(m);
//				xWingPossibilities.push_back(xWingPossibility);
//			}
//		}
//	}
//	if(xWingPossibilities >= 2) {
//		for(unsigned int p = 0; p < xWingPossibilities.size(); p++) {
//			for(unsigned int q = 0; q < xWingPossibilities.size(); q++) {
//				if(p != q &&
//					xWingPossibilities[p][1] == xWingPossibilities[q][1] &&
//					xWingPossibilities[p][2] == xWingPossibilities[q][2] &&
//					xWingPossibilities[p][3] == xWingPossibilities[q][3]) {
//						int case11I = allColumns[xWingPossibilities[p][0]][xWingPossibilities[p][1]]->i;
//						int case11J = allColumns[xWingPossibilities[p][0]][xWingPossibilities[p][1]]->j;
//						int case11K = allColumns[xWingPossibilities[p][0]][xWingPossibilities[p][1]]->k;
//						int case21I = allColumns[xWingPossibilities[p][0]][xWingPossibilities[p][2]]->i;
//						int case21J = allColumns[xWingPossibilities[p][0]][xWingPossibilities[p][2]]->i;
//						int case21K = allColumns[xWingPossibilities[p][0]][xWingPossibilities[p][2]]->i;
//						int case12I = allColumns[xWingPossibilities[q][0]][xWingPossibilities[q][1]]->i;
//						int case12J = allColumns[xWingPossibilities[q][0]][xWingPossibilities[q][1]]->j;
//						int case12K = allColumns[xWingPossibilities[q][0]][xWingPossibilities[q][1]]->k;
//						int case22I = allColumns[xWingPossibilities[q][0]][xWingPossibilities[q][2]]->i;
//						int case22J = allColumns[xWingPossibilities[q][0]][xWingPossibilities[q][2]]->i;
//						int case22K = allColumns[xWingPossibilities[q][0]][xWingPossibilities[q][2]]->i;
//
//						bool hasRemovedPossibility = false;
//
//						vector<Case*> firstLine;
//						if(case11I == 1) {
//							firstLine = getSameColumnOfCase(case11I, case11J, case11K);
//						}else {
//							firstLine = getSameLineOfCase(case11I, case11J, case11K);
//						}
//
//						for(unsigned int r = 0; r < firstLine.size(); r++) {
//							if(!(firstLine[r]->i == case11I &&
//								firstLine[r]->j == case11J &&
//								firstLine[r]->k == case11K) &&
//								!(firstLine[r]->i == case12I &&
//								firstLine[r]->j == case12J &&
//								firstLine[r]->k == case12K)) {
//									firstLine[r]->removePossibility(xWingPossibilities[q][3]);
//							}
//						}
//
//						vector<Case*> secondLine;
//						if(case11I == 1) {
//							secondLine = getSameColumnOfCase(case21I, case21J, case21K);
//						}else {
//							secondLine = getSameLineOfCase(case21I, case21J, case21K);
//						}
//
//				}
//
//
//				}
//			}
//	}
//
	return false;
}

vector<Case*> Sudokube::getSameLineOfCase(int i, int j, int k) {
	vector<Case*> sameLineCases;

	sameLineCases.push_back(container[i-1][0][k-1]);
	sameLineCases.push_back(container[i-1][1][k-1]);
	sameLineCases.push_back(container[i-1][2][k-1]);
	sameLineCases.push_back(container[i-1][3][k-1]);

	int x;
	int y;
	if (i == 1) {
		x = 2;
		y = 4-k;
	}else if (i == 2) {
		x = 0;
		y = k-1;
	} else { //i == 3
		x = 1;
		y = k-1;
	}

	sameLineCases.push_back(container[x][y][0]);
	sameLineCases.push_back(container[x][y][1]);
	sameLineCases.push_back(container[x][y][2]);
	sameLineCases.push_back(container[x][y][3]);


	return sameLineCases;
}

vector<Case*> Sudokube::getSameColumnOfCase(int i, int j, int k) {
	vector<Case*> sameColumnCases;
	sameColumnCases.push_back(container[i-1][j-1][0]);
	sameColumnCases.push_back(container[i-1][j-1][1]);
	sameColumnCases.push_back(container[i-1][j-1][2]);
	sameColumnCases.push_back(container[i-1][j-1][3]);

	int x;
	int z;
	if (i == 1) {
		x = 1;
		z = j-1;
	}else if (i == 2) {
		x = 2;
		z = j-1;
	} else { //i == 3
		x = 0;
		z = 4-j;
	}
	sameColumnCases.push_back(container[x][0][z]);
	sameColumnCases.push_back(container[x][1][z]);
	sameColumnCases.push_back(container[x][2][z]);
	sameColumnCases.push_back(container[x][3][z]);

	return sameColumnCases;
}

vector<Case*> Sudokube::getSameRegionOfCase(int i, int j, int k) {
	vector<Case*> sameRegionCases;
	if(j <= 2) {
		sameRegionCases.push_back(container[i-1][0][0]);
		sameRegionCases.push_back(container[i-1][0][1]);
		sameRegionCases.push_back(container[i-1][0][2]);
		sameRegionCases.push_back(container[i-1][0][3]);
		sameRegionCases.push_back(container[i-1][1][0]);
		sameRegionCases.push_back(container[i-1][1][1]);
		sameRegionCases.push_back(container[i-1][1][2]);
		sameRegionCases.push_back(container[i-1][1][3]);
	}else {
		sameRegionCases.push_back(container[i-1][2][0]);
		sameRegionCases.push_back(container[i-1][2][1]);
		sameRegionCases.push_back(container[i-1][2][2]);
		sameRegionCases.push_back(container[i-1][2][3]);
		sameRegionCases.push_back(container[i-1][3][0]);
		sameRegionCases.push_back(container[i-1][3][1]);
		sameRegionCases.push_back(container[i-1][3][2]);
		sameRegionCases.push_back(container[i-1][3][3]);
	}
	return sameRegionCases;
}

bool Sudokube::checkLastRemainingCellInARegion(int i, int j, int k) {
	vector<vector<Case*> > allRegions;
	allRegions.push_back(getSameLineOfCase(1,1,4));
	allRegions.push_back(getSameLineOfCase(1,1,3));
	allRegions.push_back(getSameLineOfCase(1,1,2));
	allRegions.push_back(getSameLineOfCase(1,1,1));
	allRegions.push_back(getSameColumnOfCase(1,1,1));
	allRegions.push_back(getSameColumnOfCase(1,2,1));
	allRegions.push_back(getSameColumnOfCase(1,3,1));
	allRegions.push_back(getSameColumnOfCase(1,4,1));
	allRegions.push_back(getSameColumnOfCase(2,1,1));
	allRegions.push_back(getSameColumnOfCase(2,2,1));
	allRegions.push_back(getSameColumnOfCase(2,3,1));
	allRegions.push_back(getSameColumnOfCase(2,4,1));
	allRegions.push_back(getSameRegionOfCase(1,1,1));
	allRegions.push_back(getSameRegionOfCase(1,3,1));
	allRegions.push_back(getSameRegionOfCase(2,1,1));
	allRegions.push_back(getSameRegionOfCase(2,3,1));
	allRegions.push_back(getSameRegionOfCase(3,1,1));
	allRegions.push_back(getSameRegionOfCase(3,3,1));

	for(unsigned int region = 0; region < allRegions.size(); region++) {
		for(unsigned int caseInRegion = 0; caseInRegion < allRegions[region].size(); caseInRegion++) {
			vector<int> possibilities = allRegions[region][caseInRegion]->getPossibilities();
			for (unsigned int x = 0; x < possibilities.size(); x++) {
				bool noOtherCaseHaveSamePossibility = true;
				for(unsigned int caseToCompare = 0; caseToCompare < allRegions[region].size(); caseToCompare++) {
					if(caseInRegion != caseToCompare && allRegions[region][caseToCompare]->contains(possibilities[x])) {
						noOtherCaseHaveSamePossibility = false;
						break;
					}
				}
				if(noOtherCaseHaveSamePossibility) {
					int nbPossibilities = allRegions[region][caseInRegion]->numberOfPossibilitiesRemaining();
					allRegions[region][caseInRegion]->setValue(possibilities[x]);
					if(nbPossibilities > allRegions[region][caseInRegion]->numberOfPossibilitiesRemaining()) {
						return true;
					}
				}
			}

		}

	}
	return false;
}
