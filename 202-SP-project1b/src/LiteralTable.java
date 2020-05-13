import java.util.ArrayList;

/**
 * literal과 관련된 데이터와 연산을 소유한다.
 * section 별로 하나씩 인스턴스를 할당한다.
 */
public class LiteralTable implements Cloneable{
    ArrayList<String> literalList;
    ArrayList<Integer> locationList;
    // 기타 literal, external 선언 및 처리방법을 구현한다.

    static int literalCount = 0; //literal 넣을때마다 count 해줌

    // LiteralTable 생성자 - 클래스 초기화
    public LiteralTable(){
        literalList = new ArrayList<String>();
        locationList = new ArrayList<Integer>();
    }

    /**
     * 새로운 Literal을 table에 추가한다.
     * @param literal : 새로 추가되는 literal의 label
     * @param location : 해당 literal이 가지는 주소값
     * 주의 : 만약 중복된 literal이 putLiteral을 통해서 입력된다면 이는 프로그램 코드에 문제가 있음을 나타낸다.
     * 매칭되는 주소값의 변경은 modifyLiteral()을 통해서 이루어져야 한다.
     */
    public void putLiteral(String literal, int location) {
        //입력하려는 literal이 있는지 확인 하고 없으면 add
        if(search(literal) == -1){
            literalList.add(literal);
            locationList.add(location);
            if(literal.contains("=C"))
                literalCount += 3;
            else
                literalCount += 1;
        }
    }



    /**
     * 기존에 존재하는 literal 값에 대해서 가리키는 주소값을 변경한다.
     * @param literal : 변경을 원하는 literal의 label
     * @param newLocation : 새로 바꾸고자 하는 주소값
     */
    public void modifyLiteral(String literal, int newLocation) {
        int index = literalList.indexOf(literal);
        locationList.set(index, newLocation);
    }

    /**
     * 인자로 전달된 literal이 어떤 주소를 지칭하는지 알려준다.
     * @param literal : 검색을 원하는 literal의 label
     * @return literal이 가지고 있는 주소값. 해당 literal이 없을 경우 -1 리턴
     */
    public int search(String literal) {

        int address = -1;
        int exist = literalList.indexOf(literal);
        if(exist != -1){
            address = locationList.get(literalList.indexOf(literal));
            return address;
        }
        else {
            return address;
        }

    }

    public String getlitAscii(int addr){
        String getAscii = "";
        int findIdx = locationList.indexOf(addr);
        String findLit = literalList.get(findIdx);
        if(findLit.charAt(1) == 'C'){
            for(int i = 0; i < 3; i++){
                getAscii += Integer.toHexString(findLit.charAt(i+3)).toUpperCase();
            }
        }
        else{
            findLit = findLit.substring(3, 5);
            getAscii += findLit;
        }
        return getAscii;
    }

    @Override
    public Object clone() throws CloneNotSupportedException{
        return super.clone();
    }


}
