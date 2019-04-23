import java.util.HashMap;

public class TwoSum {
    private static int[] twoSum(int[] nums, int target) {
        HashMap<Integer, Integer> map = new HashMap<>();
        
        for (int i = 0; i < nums.length; i++)
            map.put(nums[i], i);
        
        for (int i = 0; i < nums.length; i++) {
            int cmp = target - nums[i];
            
            if (map.containsKey(cmp) && map.get(cmp) != i)
                return new int[] {i, map.get(cmp)};
        }
        throw new IllegalArgumentException("invalid input");
    }

    public static void main(String[] args) {
        int[] nums = {2, 7, 11, 15};
        int target = 9;
        int[] result = twoSum(nums, target);

        System.out.println("Indices:");
        for (int i = 0; i < result.length; i++) {
            System.out.println(result[i]);
        }
    }
}