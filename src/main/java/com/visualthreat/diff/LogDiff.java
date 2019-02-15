package com.visualthreat.diff;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.PrintWriter;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import lombok.extern.slf4j.Slf4j;

@Slf4j
public class LogDiff {

	private static final int CHECK_RANGE = 2;
	private static final double INGNORE_FACTOR = 0.01;
	private static final String DATA_REG = "(\"data\":)\\[(.*)\\]";
	private static final String ID_REG = "(\"id\":)(0[xX][0-9a-fA-F]+)";
	private static final String TIME_REG = "(\"timestamp\":)([0-9]+)";
	private static final String DLC_REG = "(\"dlc\":)([0-9]+)";

	public static void findDiff(String file1, String file2, String file3) {

		List<List<String>> base = loadFile(file1);
		List<List<String>> act = loadFile(file2);
		Map<String, Map<String, Integer>> relationMap = buildMap(base);
		Map<String, Integer> freq = countIdFreq(act);


		for (Entry<String, Integer> entry : freq.entrySet()) {
			System.out.println(entry.getKey() + " : " + entry.getValue());

		}

		int[] frames = new int[act.size()];
		for (int i = 0; i < act.size(); i++) {
			String frame = act.get(i).get(0);
			// new can id
			if (!relationMap.containsKey(frame)) {
				frames[i] = -1;
				continue;
			}
			for (int j = 1; j <= CHECK_RANGE; j++) {
				// check for next few neighbors
				if (i + j < act.size()) {
					String nei = act.get(i + j).get(0);
					if (relationMap.containsKey(nei) && relationMap.get(nei).containsKey(frame)) {
						frames[i] += 1;
					} else {
						frames[i] -= 1;
					}
				}
				// check previous few neighbors
				if (i - j >= 0) {
					String nei = act.get(i - j).get(0);
					if (relationMap.containsKey(nei) && relationMap.get(nei).containsKey(frame)) {
						frames[i] += 1;
					} else {
						frames[i] -= 1;
					}
				}
			}
		}

		// Generate the result
		PrintWriter printWriter = null;
		try {
			printWriter = new PrintWriter(new File(file3));
			// filte same result
			Set<String> filter = new HashSet<>();
			for (int i = 0; i < act.size(); i++) {
				String key = act.get(i).get(0) + "," + act.get(i).get(1);
				if (frames[i] < 0 && freq.get(act.get(i).get(0)) < act.size() * INGNORE_FACTOR && !filter.contains(key)) {
					printWriter.print((String.format("{\"timestamp\":%s,\"id\":%s,\"dlc\":%s,\"data\":[%s]}\n",
							act.get(i).get(2), act.get(i).get(0),act.get(i).get(3),act.get(i).get(1))));
					filter.add(key);
				}
			}
			printWriter.close();
		} catch (Exception e) {
			log.error("Error generate the result");
		} finally {
			printWriter.close();
		}
	}

	private static List<List<String>> loadFile(String path) {
		List<List<String>> res = new ArrayList<>();
		try {
			BufferedReader bufferedReader = new BufferedReader(new FileReader(path));
			String line = null;
			Pattern p1 = Pattern.compile(ID_REG);
			Pattern p2 = Pattern.compile(DATA_REG);
			Pattern p3 = Pattern.compile(TIME_REG);
			Pattern p4 = Pattern.compile(DLC_REG);
			while ((line = bufferedReader.readLine()) != null) {
				Matcher matcher1 = p1.matcher(line);
				Matcher matcher2 = p2.matcher(line);
				Matcher matcher3 = p3.matcher(line);
				Matcher matcher4 = p4.matcher(line);
				if(!(matcher1.find() && matcher2.find() && matcher3.find() && matcher4.find())) {
					continue;
				}
				res.add(new ArrayList<>(Arrays.asList(matcher1.group(2), matcher2.group(2), matcher3.group(2), matcher4.group(2))));
			}
			bufferedReader.close();
		} catch (Exception e) {
			log.error("Error Reading From traffic file", e);
		}
		return res;
	}

	private static Map<String, Map<String, Integer>> buildMap(List<List<String>> frames) {

		Map<String, Map<String, Integer>> map = new HashMap<>();

		for (int i = 0; i < frames.size(); i++) {
			String canID = frames.get(i).get(0);
			if (!map.containsKey(canID)) {
				map.put(canID, new HashMap<>());
			}
			for (int j = 1; j <= CHECK_RANGE; j++) {
				// scan the next j canID
				if (i + j < frames.size()) {
					int count = map.get(canID).getOrDefault(frames.get(i + j).get(0), 0);
					map.get(canID).put(frames.get(i + j).get(0), count + 1);
				}
				// scan the previous j canID
				if (i - j >= 0) {
					int count = map.get(canID).getOrDefault(frames.get(i - j).get(0), 0);
					map.get(canID).put(frames.get(i - j).get(0), count + 1);
				}
			}
		}
		return map;
	}

	/**
	 * Helper function used to count the frequeny of each ID
	 * @param frames
	 * @return
	 */
	private static Map<String, Integer> countIdFreq(List<List<String>> frames) {
		Map<String, Integer> freq = new HashMap<>();
		for (List<String> frame : frames) {
			String key = frame.get(0);
			int count = freq.getOrDefault(key, 0);
			freq.put(key, count + 1);
		}
		return freq;
	}

	/**
	 * Test
	 * @param args
	 */
	public static void main(String[] args) {

		LogDiff.findDiff("kill_car.traffic", "PSCM.traffic", "output.traffic");

	}
}

